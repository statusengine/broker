#pragma once

#include <memory>
#include <ctime>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "INebmodule.h"
#include "IStatusengine.h"
#include "MessageHandler/IMessageHandler.h"
#include "ObjectSerializerImpl.h"


namespace statusengine {

    class NebmoduleCallback {
      public:
        explicit NebmoduleCallback(NEBCallbackType cbType, IStatusengine &se) : se(se), cbType(cbType) {}

        NebmoduleCallback(statusengine::NebmoduleCallback &&other) noexcept
                : se(other.se), cbType(other.cbType) {}

        virtual ~NebmoduleCallback() = default;

        virtual NEBCallbackType GetCallbackType() {
            return cbType;
        }

        virtual void Callback(int event_type, void *data) = 0;

      protected:
        IStatusengine &se;
        NEBCallbackType cbType;
    };

    template <typename Nebtype, typename Desttype, NEBCallbackType CBT, Queue queue>
    class StandardCallback : public NebmoduleCallback {
    public:
        explicit StandardCallback(IStatusengine &se) : NebmoduleCallback(CBT, se) {
            qHandler = se.GetMessageHandler()->GetMessageQueueHandler(queue);
        }

        StandardCallback(StandardCallback &&other) noexcept
                : NebmoduleCallback::NebmoduleCallback(std::move(other)), qHandler(std::move(other.qHandler)) {}

        void Callback(int, void *data) override {
            auto nData = reinterpret_cast<Nebtype *>(data);
            Desttype dData(se.GetNebmodule(), nData);
            qHandler->SendMessage(dData);
        }

    protected:
        std::shared_ptr<IMessageQueueHandler> qHandler;
    };

    template <typename Nebtype, typename Serializer, NEBCallbackType CBT, Queue queue>
    class StandardCallbackSerializer : public NebmoduleCallback {
    public:
        explicit StandardCallbackSerializer(IStatusengine &se) : NebmoduleCallback(CBT, se), serializer(se.GetNebmodule()) {
            qHandler = se.GetMessageHandler()->GetMessageQueueHandler(queue);
        }

        StandardCallbackSerializer(StandardCallbackSerializer &&other) noexcept
                : NebmoduleCallback::NebmoduleCallback(std::move(other)), qHandler(std::move(other.qHandler)), serializer(std::move(other.serializer)) {}

        void Callback(int, void *data) override {
            qHandler->SendMessage(serializer.ToJson(*(reinterpret_cast<const Nebtype *>(data))));
        }

    protected:
        std::shared_ptr<IMessageQueueHandler> qHandler;
        Serializer serializer;
    };

    class ServiceCheckCallback : public NebmoduleCallback {
    public:
        explicit ServiceCheckCallback(IStatusengine &se)
                : NebmoduleCallback(NEBCALLBACK_SERVICE_CHECK_DATA, se), servicechecks(false), ocsp(false),
                  service_perfdata(false), checkDataSerializer(se.GetNebmodule()), perfDataSerializer(se.GetNebmodule()) {
            auto mHandler = se.GetMessageHandler();
            if (mHandler->QueueExists(Queue::ServiceCheck)) {
                serviceCheckHandler = mHandler->GetMessageQueueHandler(Queue::ServiceCheck);
                servicechecks = true;
            }
            if (mHandler->QueueExists(Queue::OCSP)) {
                ocspHandler = mHandler->GetMessageQueueHandler(Queue::OCSP);
                ocsp = true;
            }
            if (mHandler->QueueExists(Queue::ServicePerfData)) {
                servicePerfHandler = mHandler->GetMessageQueueHandler(Queue::ServicePerfData);
                service_perfdata = true;
            }
        }

        ServiceCheckCallback(ServiceCheckCallback &&other) noexcept
                : NebmoduleCallback::NebmoduleCallback(std::move(other)), servicechecks(other.servicechecks), ocsp(other.ocsp),
                  service_perfdata(other.service_perfdata), checkDataSerializer(std::move(other.checkDataSerializer)), perfDataSerializer(std::move(other.perfDataSerializer)),
                  serviceCheckHandler(std::move(other.serviceCheckHandler)), ocspHandler(std::move(other.ocspHandler)),
                  servicePerfHandler(std::move(other.servicePerfHandler)) {}

        void Callback(int, void *vdata) override {
            auto data = reinterpret_cast<nebstruct_service_check_data *>(vdata);
            auto temp_service = reinterpret_cast<service*>(data->object_ptr);

            if (data->type == NEBTYPE_SERVICECHECK_PROCESSED) {
                if (servicechecks || ocsp) {
                    auto checkData = checkDataSerializer.ToJson(*data);
                    if (servicechecks) {
                        serviceCheckHandler->SendMessage(checkData);
                    }
                    if (ocsp) {
                        ocspHandler->SendMessage(checkData);
                    }
                }
                if (service_perfdata && temp_service->process_performance_data != 0) {
                    servicePerfHandler->SendMessage(perfDataSerializer.ToJson(*data));
                }
            }
        }

    private:
        bool servicechecks;
        bool ocsp;
        bool service_perfdata;
        NagiosServiceCheckDataSerializer checkDataSerializer;
        NagiosServiceCheckPerfDataSerializer perfDataSerializer;
        std::shared_ptr<IMessageQueueHandler> serviceCheckHandler;
        std::shared_ptr<IMessageQueueHandler> ocspHandler;
        std::shared_ptr<IMessageQueueHandler> servicePerfHandler;
    };

    class ProcessDataCallback : public NebmoduleCallback {
    public:
        explicit ProcessDataCallback(IStatusengine &se)
                : NebmoduleCallback(NEBCALLBACK_PROCESS_DATA, se), restartData(false), processData(false),
                  startupSchedulerMax(se.GetStartupScheduleMax()), serializer(se.GetNebmodule()) {
            auto mHandler = se.GetMessageHandler();
            if (mHandler->QueueExists(Queue::RestartData)) {
                restartHandler = mHandler->GetMessageQueueHandler(Queue::RestartData);
                restartData = true;
            }
            if (mHandler->QueueExists(Queue::ProcessData)) {
                processHandler = mHandler->GetMessageQueueHandler(Queue::ProcessData);
                processData = true;
            }
        }

        ProcessDataCallback(ProcessDataCallback &&other) noexcept
                : NebmoduleCallback::NebmoduleCallback(std::move(other)), restartData(other.restartData),
                  processData(other.processData), startupSchedulerMax(other.startupSchedulerMax), serializer(std::move(other.serializer)),
                  restartHandler(std::move(other.restartHandler)), processHandler(std::move(other.processHandler)) {}

        void Callback(int, void *vdata) override {
            auto data = reinterpret_cast<nebstruct_process_data *>(vdata);

            if (data->type == NEBTYPE_PROCESS_START) {
                se.InitEventCallbacks();
                if (restartData) {
                    rapidjson::StringBuffer msgData;
                    rapidjson::Writer<rapidjson::StringBuffer> msgObj(msgData);
                    msgObj.StartObject();
                    msgObj.Key("object_type");
                    msgObj.Int64(NEBTYPE_PROCESS_RESTART);
                    msgObj.EndObject();
                    restartHandler->SendMessage(msgData.GetString());
                }
            }

            if (startupSchedulerMax > 0 && data->type == NEBTYPE_PROCESS_EVENTLOOPSTART) {
                se.Log() << "Reschedule all hosts and services" << LogLevel::Info;
                for (auto temp_host = host_list; temp_host != nullptr; temp_host = temp_host->next) {
                    auto now = std::time(nullptr);
                    time_t check_interval = static_cast<time_t>(temp_host->check_interval) * interval_length;
                    time_t delay;
                    if ((now - temp_host->last_check) > check_interval) {
                        delay = ranged_urand(0, static_cast<unsigned int>(std::min(startupSchedulerMax, check_interval)));
                    }
                    else {
                        delay = check_interval - (now - temp_host->last_check);
                    }
                    se.GetNebmodule().ScheduleHostCheckDelay(temp_host, delay);
                }

                for (auto temp_service = service_list; temp_service != nullptr; temp_service = temp_service->next) {
                    auto now = std::time(nullptr);
                    time_t check_interval = static_cast<time_t>(temp_service->check_interval) * interval_length;
                    time_t delay;
                    if ((now - temp_service->last_check) > check_interval) {
                        delay = ranged_urand(0, static_cast<unsigned int>(std::min(startupSchedulerMax, check_interval)));
                    }
                    else {
                        delay = check_interval - (now - temp_service->last_check);
                    }
                    se.GetNebmodule().ScheduleServiceCheckDelay(temp_service, delay);
                }
                se.Log() << "Reschedule complete" << LogLevel::Info;
            }

            if (processData) {
                auto processDataMsg = serializer.ToJson(*data);
                processHandler->SendMessage(processDataMsg);
            }
        }

    private:
        bool restartData;
        bool processData;
        time_t startupSchedulerMax;
        NagiosProcessDataSerializer serializer;

        std::shared_ptr<IMessageQueueHandler> restartHandler;
        std::shared_ptr<IMessageQueueHandler> processHandler;
    };

    class HostCheckCallback : public NebmoduleCallback {
    public:
        explicit HostCheckCallback(IStatusengine &se)
                : NebmoduleCallback(NEBCALLBACK_HOST_CHECK_DATA, se), hostchecks(false), ochp(false), serializer(se.GetNebmodule()) {
            auto mHandler = se.GetMessageHandler();
            if (mHandler->QueueExists(Queue::HostCheck)) {
                hostCheckHandler = mHandler->GetMessageQueueHandler(Queue::HostCheck);
                hostchecks = true;
            }
            if (mHandler->QueueExists(Queue::OCHP)) {
                ochpHandler = mHandler->GetMessageQueueHandler(Queue::OCHP);
                ochp = true;
            }
        }

        HostCheckCallback(HostCheckCallback &&other) noexcept
                : NebmoduleCallback::NebmoduleCallback(std::move(other)), hostchecks(other.hostchecks), ochp(other.ochp), serializer(other.serializer),
                  hostCheckHandler(std::move(other.hostCheckHandler)), ochpHandler(std::move(other.ochpHandler)) {}

        void Callback(int, void *vdata) override {
            auto data = reinterpret_cast<nebstruct_host_check_data *>(vdata);

            if (data->type == NEBTYPE_HOSTCHECK_PROCESSED) {
                auto msg = serializer.ToJson(*data);
                if (hostchecks) {
                    hostCheckHandler->SendMessage(msg);
                }
                if (ochp) {
                    ochpHandler->SendMessage(msg);
                }
            }
        }

    private:
        bool hostchecks;
        bool ochp;
        NagiosHostCheckDataSerializer serializer;

        std::shared_ptr<IMessageQueueHandler> hostCheckHandler;
        std::shared_ptr<IMessageQueueHandler> ochpHandler;
    };

} // namespace statusengine
