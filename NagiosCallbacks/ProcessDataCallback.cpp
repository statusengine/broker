#include "ProcessDataCallback.h"

#include <algorithm>

#include "LogStream.h"
#include "NagiosObjects/NagiosProcessData.h"
#include "Statusengine.h"

namespace statusengine {
    ProcessDataCallback::ProcessDataCallback(Statusengine *se, bool restartData, bool processData,
                                             time_t startupSchedulerMax)
        : NebmoduleCallback(NEBCALLBACK_PROCESS_DATA, se), restartData(restartData), processData(processData),
          startupSchedulerMax(startupSchedulerMax) {}

    void ProcessDataCallback::Callback(int event_type, nebstruct_process_data *data) {
        if (restartData && data->type == NEBTYPE_PROCESS_START) {
            json_object *restartData = json_object_new_object();
            json_object_object_add(restartData, "object_type", json_object_new_int(NEBTYPE_PROCESS_RESTART));
            se->SendMessage("statusngin_core_restart", std::string(json_object_to_json_string(restartData)));
            json_object_put(restartData);
        }

        if (startupSchedulerMax > 0 && data->type == NEBTYPE_PROCESS_EVENTLOOPSTART) {
            se->Log() << "Reschedule all hosts and services" << eom;
            for (auto temp_host = host_list; temp_host != NULL; temp_host = temp_host->next) {
                auto now = std::time(nullptr);
                auto check_interval = get_host_check_interval_s(temp_host);
                time_t delay;
                if ((now - temp_host->last_check) > check_interval) {
                    delay = ranged_urand(0, std::min(startupSchedulerMax, check_interval));
                }
                else {
                    delay = check_interval - (now - temp_host->last_check);
                }
                schedule_next_host_check(temp_host, delay, CHECK_OPTION_NONE);
            }

            for (auto temp_service = service_list; temp_service != NULL; temp_service = temp_service->next) {
                auto now = std::time(nullptr);
                auto check_interval = get_service_check_interval_s(temp_service);
                time_t delay;
                if ((now - temp_service->last_check) > check_interval) {
                    delay = ranged_urand(0, std::min(startupSchedulerMax, check_interval));
                }
                else {
                    delay = check_interval - (now - temp_service->last_check);
                }
                schedule_next_service_check(temp_service, delay, CHECK_OPTION_NONE);
            }
            se->Log() << "Reschedule complete" << eom;
        }

        if (processData) {
            NagiosProcessData processData(data);
            se->SendMessage("statusngin_processdata", processData.ToString());
        }
    }
} // namespace statusengine
