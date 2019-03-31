#pragma once

#include <memory>
#include <ctime>

#include "Nebmodule.h"
#include "IStatusengine.h"
#include "MessageHandler/IMessageQueueHandler.h"
#include "NagiosObject.h"


namespace statusengine {

    class NebmoduleCallback {
      public:
        explicit NebmoduleCallback(NEBCallbackType cbType, IStatusengine &se) : cbType(cbType), se(se) {}

        NebmoduleCallback(statusengine::NebmoduleCallback &&other) noexcept
                : cbType(other.cbType), se(other.se) {}

        virtual ~NebmoduleCallback() = default;

        virtual NEBCallbackType GetCallbackType() {
            return cbType;
        }

        virtual void Callback(int event_type, void *data) = 0;

      protected:
        IStatusengine &se;
        NEBCallbackType cbType;
    };

    template <typename N, typename D, NEBCallbackType CBT, Queue queue>
    class StandardCallback : public NebmoduleCallback {
    public:
        explicit StandardCallback(IStatusengine &se) : NebmoduleCallback(CBT, se) {
            qHandler = se.GetMessageHandler()->GetMessageQueueHandler(queue);
        }

        StandardCallback(StandardCallback &&other) noexcept
                : NebmoduleCallback::NebmoduleCallback(std::move(other)), qHandler(std::move(other.qHandler)) {}

        void Callback(int event_type, void *data) override {
            auto nData = reinterpret_cast<N *>(data);
            D dData(nData);
            qHandler->SendMessage(dData);
        }

    protected:
        std::shared_ptr<IMessageQueueHandler> qHandler;
    };

    class ServiceCheckCallback : public NebmoduleCallback {
    public:
        explicit ServiceCheckCallback(IStatusengine &se)
                : NebmoduleCallback(NEBCALLBACK_SERVICE_CHECK_DATA, se), servicechecks(false), ocsp(false),
                  service_perfdata(false) {
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
                  service_perfdata(other.service_perfdata), serviceCheckHandler(std::move(other.serviceCheckHandler)),
                  ocspHandler(std::move(other.ocspHandler)), servicePerfHandler(std::move(other.servicePerfHandler)) {}

        void Callback(int event_type, void *vdata) override {
            auto data = reinterpret_cast<nebstruct_service_check_data *>(vdata);
            auto temp_service = reinterpret_cast<service*>(data->object_ptr);

            if (data->type == NEBTYPE_SERVICECHECK_PROCESSED) {
                if (servicechecks || ocsp) {
                    NagiosServiceCheckData checkData(data);
                    ;
                    if (servicechecks) {
                        serviceCheckHandler->SendMessage(checkData);
                    }
                    if (ocsp) {
                        ocspHandler->SendMessage(checkData);
                    }
                }
                if (service_perfdata && temp_service->process_performance_data != 0) {
                    NagiosServiceCheckPerfData checkPerfData(data);
                    servicePerfHandler->SendMessage(checkPerfData);
                }
            }
        }

    private:
        bool servicechecks;
        bool ocsp;
        bool service_perfdata;
        std::shared_ptr<IMessageQueueHandler> serviceCheckHandler;
        std::shared_ptr<IMessageQueueHandler> ocspHandler;
        std::shared_ptr<IMessageQueueHandler> servicePerfHandler;
    };

    class ProcessDataCallback : public NebmoduleCallback {
    public:
        explicit ProcessDataCallback(IStatusengine &se, time_t startupSchedulerMax)
                : NebmoduleCallback(NEBCALLBACK_PROCESS_DATA, se), restartData(false), processData(false),
                  startupSchedulerMax(startupSchedulerMax) {
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
                  processData(other.processData), startupSchedulerMax(other.startupSchedulerMax),
                  restartHandler(std::move(other.restartHandler)), processHandler(std::move(other.processHandler)) {}

        void Callback(int event_type, void *vdata) override {
            auto data = reinterpret_cast<nebstruct_process_data *>(vdata);

            if (data->type == NEBTYPE_PROCESS_START) {
                se.InitEventCallbacks();
                if (restartData) {
                    NagiosObject msgObj;
                    msgObj.SetData("object_type", static_cast<int>(NEBTYPE_PROCESS_RESTART));
                    restartHandler->SendMessage(msgObj);
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
                    Nebmodule::ScheduleHostCheckDelay(temp_host, delay);
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
                    Nebmodule::ScheduleServiceCheckDelay(temp_service, delay);
                }
                se.Log() << "Reschedule complete" << LogLevel::Info;
            }

            if (processData) {
                NagiosProcessData processDataMsg(data);
                processHandler->SendMessage(processDataMsg);
            }
        }

    private:
        bool restartData;
        bool processData;
        time_t startupSchedulerMax;

        std::shared_ptr<IMessageQueueHandler> restartHandler;
        std::shared_ptr<IMessageQueueHandler> processHandler;
    };

    class HostCheckCallback : public NebmoduleCallback {
    public:
        explicit HostCheckCallback(IStatusengine &se)
                : NebmoduleCallback(NEBCALLBACK_HOST_CHECK_DATA, se), hostchecks(false), ochp(false) {
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
                : NebmoduleCallback::NebmoduleCallback(std::move(other)), hostchecks(other.hostchecks), ochp(other.ochp),
                  hostCheckHandler(std::move(other.hostCheckHandler)), ochpHandler(std::move(other.ochpHandler)) {}

        void Callback(int event_type, void *vdata) override {
            auto data = reinterpret_cast<nebstruct_host_check_data *>(vdata);

            if (data->type == NEBTYPE_HOSTCHECK_PROCESSED) {
                NagiosHostCheckData checkData(data);
                if (hostchecks) {
                    hostCheckHandler->SendMessage(checkData);
                }
                if (ochp) {
                    ochpHandler->SendMessage(checkData);
                }
            }
        }

    private:
        bool hostchecks;
        bool ochp;

        std::shared_ptr<IMessageQueueHandler> hostCheckHandler;
        std::shared_ptr<IMessageQueueHandler> ochpHandler;
    };

} // namespace statusengine
