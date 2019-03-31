#include "ProcessDataCallback.h"

#include <algorithm>

#include "MessageHandler/MessageHandlerList.h"
#include "Statusengine.h"

namespace statusengine {
    ProcessDataCallback::ProcessDataCallback(Statusengine *se, time_t startupSchedulerMax)
        : NebmoduleCallback(NEBCALLBACK_PROCESS_DATA, se), restartData(false), processData(false),
          startupSchedulerMax(startupSchedulerMax) {
        auto mHandler = se->GetMessageHandler();
        if (mHandler->QueueExists(Queue::RestartData)) {
            restartHandler = mHandler->GetMessageQueueHandler(Queue::RestartData);
            restartData = true;
        }
        if (mHandler->QueueExists(Queue::ProcessData)) {
            processHandler = mHandler->GetMessageQueueHandler(Queue::ProcessData);
            processData = true;
        }
    }

    ProcessDataCallback::ProcessDataCallback(ProcessDataCallback &&other) noexcept
        : NebmoduleCallback::NebmoduleCallback(std::move(other)), restartData(other.restartData),
          processData(other.processData), startupSchedulerMax(other.startupSchedulerMax),
          restartHandler(std::move(other.restartHandler)), processHandler(std::move(other.processHandler)) {}

    void ProcessDataCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_process_data *>(vdata);

        if (data->type == NEBTYPE_PROCESS_START) {
            se->InitEventCallbacks();
            if (restartData) {
                NagiosObject msgObj;
                msgObj.SetData("object_type", static_cast<int>(NEBTYPE_PROCESS_RESTART));
                restartHandler->SendMessage(msgObj);
            }
        }

        if (startupSchedulerMax > 0 && data->type == NEBTYPE_PROCESS_EVENTLOOPSTART) {
            se->Log() << "Reschedule all hosts and services" << LogLevel::Info;
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
            se->Log() << "Reschedule complete" << LogLevel::Info;
        }

        if (processData) {
            NagiosProcessData processData(data);
            processHandler->SendMessage(processData);
        }
    }
} // namespace statusengine
