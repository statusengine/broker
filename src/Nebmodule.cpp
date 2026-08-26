#include "Nebmodule.h"

#include <ctime>
#include <string>
#include <cstring>

#include "EventCallback.h"
#include "gsl.h"
#include "Statusengine.h"

// This is required by naemon
extern "C" {
NEB_API_VERSION(CURRENT_NEB_API_VERSION)
}

namespace statusengine {

    int Nebmodule::Init(nebmodule *handle, std::string args) {
        se = new Statusengine(handle, std::move(args));
        encoder.SetWarnCallback([this](const std::string &message) {
            se->Log() << message << LogLevel::Warning;
        });
        return se->Init();
    }

    int Nebmodule::Deinit(int) {
        // The encoder outlives se, so it must not keep logging through it.
        encoder.SetWarnCallback(nullptr);
        delete se;
        se = nullptr;
        return 0;
    }

    int Nebmodule::Callback(int event_type, void *data) {
        return se->Callback(event_type, data);
    }

    bool Nebmodule::RegisterCallback(NEBCallbackType cbType) {
        int result = neb_register_callback(cbType, se->nebhandle, 0, nebmodule_callback);

        if (result != 0) {
            se->Log() << "Could not register nebmodule_callback for Event Type " << cbType << ": " << result
                      << LogLevel::Error;
            return false;
        }
        else {
            se->Log() << "Register nebmodule_callback for Event Type: " << cbType << LogLevel::Info;
            return true;
        }
    }

    void Nebmodule::RegisterEventCallback(EventCallback *ecb) {
#ifndef BUILD_NAGIOS
        schedule_event(static_cast<time_t>(ecb->Interval()), nebmodule_event_callback, ecb);
#else
        time_t interval = static_cast<time_t>(ecb->Interval());
        schedule_new_event(EVENT_USER_FUNCTION, 1, std::time(0) + interval, 1, interval, nullptr, 1,
                           reinterpret_cast<void *>(nebmodule_event_callback), reinterpret_cast<void *>(ecb), 0);
#endif // BUILD_NAGIOS
    }

#ifndef BUILD_NAGIOS
    void Nebmodule::RegisterEventCallbackNow(EventCallback *ecb) {
        // A zero delay does not starve the core. naemon's event_poll_full() computes the
        // time to the next event, clamps it to zero for one that is already due, polls its
        // own file descriptors with that timeout, and skips running the timed event
        // altogether if any of them had input. So the next worker slice only happens once
        // naemon has had a pass of its own, and naemon's own I/O gets priority over ours.
        schedule_event(0, nebmodule_event_callback, ecb);
    }
#endif

    void Nebmodule::ScheduleHostCheckDelay(host *temp_host, time_t delay) {
#ifndef BUILD_NAGIOS
        schedule_next_host_check(temp_host, delay, CHECK_OPTION_NONE);
#else
        time_t schedule_time = std::time(nullptr) + delay;
        schedule_host_check(temp_host, schedule_time, CHECK_OPTION_NONE);
#endif // BUILD_NAGIOS
    }

    void Nebmodule::ScheduleHostCheckFixed(host *temp_host, time_t schedule_time) {
#ifndef BUILD_NAGIOS
        time_t delay = schedule_time - std::time(nullptr);
        if (delay < 0) {
            delay = 0;
        }
        schedule_next_host_check(temp_host, delay, CHECK_OPTION_NONE);
#else
        schedule_host_check(temp_host, schedule_time, CHECK_OPTION_NONE);
#endif // BUILD_NAGIOS
    }

    void Nebmodule::ScheduleServiceCheckDelay(service *temp_service, time_t delay) {
#ifndef BUILD_NAGIOS
        schedule_next_service_check(temp_service, delay, CHECK_OPTION_NONE);
#else
        time_t schedule_time = std::time(nullptr) + delay;
        schedule_service_check(temp_service, schedule_time, CHECK_OPTION_NONE);
#endif // BUILD_NAGIOS
    }

    void Nebmodule::ScheduleServiceCheckFixed(service *temp_service, time_t schedule_time) {
#ifndef BUILD_NAGIOS
        time_t delay = schedule_time - std::time(nullptr);
        if (delay < 0) {
            delay = 0;
        }
        schedule_next_service_check(temp_service, delay, CHECK_OPTION_NONE);
#else
        schedule_service_check(temp_service, schedule_time, CHECK_OPTION_NONE);
#endif // BUILD_NAGIOS
    }

    void Nebmodule::DeleteDowntime(const char *hostname, const char *service_description, time_t start_time, time_t end_time, const char *comment) {
#ifndef BUILD_NAGIOS
        scheduled_downtime *temp_downtime;
        scheduled_downtime *next_downtime;
        void *downtime_cpy;
        objectlist *matches = NULL, *tmp_match = NULL;

        /* Do not allow deletion of everything - must have at least 1 filter on */
        if (hostname == NULL && service_description == NULL && start_time == 0 && end_time == 0 && comment == NULL)
            return;

        for (temp_downtime = scheduled_downtime_list; temp_downtime != NULL; temp_downtime = next_downtime) {
            next_downtime = temp_downtime->next;
            if (start_time != 0 && temp_downtime->start_time != start_time) {
                continue;
            }
            if (end_time != 0 && temp_downtime->end_time != end_time) {
                continue;
            }
            if (comment != NULL && std::strcmp(temp_downtime->comment, comment) != 0)
                continue;
            if (temp_downtime->type == HOST_DOWNTIME) {
                /* If service is specified, then do not delete the host downtime */
                if (service_description != NULL)
                    continue;
                if (hostname != NULL && std::strcmp(temp_downtime->host_name, hostname) != 0)
                    continue;
            } else if (temp_downtime->type == SERVICE_DOWNTIME) {
                if (hostname != NULL && std::strcmp(temp_downtime->host_name, hostname) != 0)
                    continue;
                if (service_description != NULL && std::strcmp(temp_downtime->service_description, service_description) != 0)
                    continue;
            }

            downtime_cpy = nm_malloc(sizeof(scheduled_downtime));
            std::memcpy(downtime_cpy, temp_downtime, sizeof(scheduled_downtime));
            prepend_object_to_objectlist(&matches, downtime_cpy);
            if (temp_downtime->type == HOST_DOWNTIME) {
                se->Log() << "Delete Host Downtime of Host '" << temp_downtime->host_name << "' with start time: "
                    << temp_downtime->start_time << " and end time: " << temp_downtime->end_time << " and comment: '"
                    << temp_downtime->comment << "'" << LogLevel::Info;
            }
            else {
                se->Log() << "Delete Service Downtime of Host'" << temp_downtime->host_name
                    << "' with service description '" << temp_downtime->service_description << "' with start time: "
                    << temp_downtime->start_time << " and end time: " << temp_downtime->end_time << " and comment: '"
                    << temp_downtime->comment << "'" << LogLevel::Info;
            }
        }

        for (tmp_match = matches; tmp_match != NULL; tmp_match = tmp_match->next) {
            temp_downtime = (scheduled_downtime *)tmp_match->object_ptr;
            unschedule_downtime(temp_downtime->type, temp_downtime->downtime_id);
            nm_free(temp_downtime);
        }

        free_objectlist(&matches);
#else
        scheduled_downtime *temp_downtime;
        scheduled_downtime *next_downtime;
        void *downtime_cpy;
        objectlist *matches = NULL, *tmp_match = NULL;

        /* Do not allow deletion of everything - must have at least 1 filter on */
        if(hostname == NULL && service_description == NULL && start_time == 0 && end_time == 0 && comment == NULL)
            return;

        for (temp_downtime = scheduled_downtime_list; temp_downtime != NULL; temp_downtime = next_downtime) {
            next_downtime = temp_downtime->next;
            if (start_time != 0 && temp_downtime->start_time != start_time) {
                continue;
            }
            if (end_time != 0 && temp_downtime->end_time != end_time) {
                continue;
            }
            if (comment != NULL && strcmp(temp_downtime->comment, comment) != 0)
                continue;
            if (temp_downtime->type == HOST_DOWNTIME) {
                /* If service is specified, then do not delete the host downtime */
                if(service_description != NULL)
                    continue;
                if(hostname != NULL && strcmp(temp_downtime->host_name, hostname) != 0)
                    continue;
                }
            else if (temp_downtime->type == SERVICE_DOWNTIME) {
                if(hostname != NULL && strcmp(temp_downtime->host_name, hostname) != 0)
                    continue;
                if (service_description != NULL && strcmp(temp_downtime->service_description, service_description) != 0)
                    continue;
            }

            downtime_cpy = malloc(sizeof(scheduled_downtime));
            memcpy(downtime_cpy, temp_downtime, sizeof(scheduled_downtime));
            prepend_object_to_objectlist(&matches, downtime_cpy);
            if (temp_downtime->type == HOST_DOWNTIME) {
                se->Log() << "Delete Host Downtime of Host '" << temp_downtime->host_name << "' with start time: "
                    << temp_downtime->start_time << " and end time: " << temp_downtime->end_time << " and comment: '"
                    << temp_downtime->comment << "'" << LogLevel::Info;
            }
            else {
                se->Log() << "Delete Service Downtime of Host'" << temp_downtime->host_name
                    << "' with service description '" << temp_downtime->service_description << "' with start time: "
                    << temp_downtime->start_time << " and end time: " << temp_downtime->end_time << " and comment: '"
                    << temp_downtime->comment << "'" << LogLevel::Info;
            }
        }

        for (tmp_match = matches; tmp_match != NULL; tmp_match = tmp_match->next) {
            temp_downtime = (scheduled_downtime *)tmp_match->object_ptr;
            unschedule_downtime(temp_downtime->type, temp_downtime->downtime_id);
            my_free(temp_downtime);
        }

        free_objectlist(&matches);

        return;

#endif // BUILD_NAGIOS
    }

    std::string Nebmodule::EncodeString(const char *inputData) {
        return encoder.ToUtf8(inputData);
    }
} // namespace statusengine

extern "C" int nebmodule_init(int, char *args, nebmodule *handle) {
    return statusengine::Nebmodule::Instance().Init(handle, std::string(args));
}

extern "C" int nebmodule_deinit(int, int reason) {
    return statusengine::Nebmodule::Instance().Deinit(reason);
}

int nebmodule_callback(int event_type, void *data) {
    return statusengine::Nebmodule::Instance().Callback(event_type, data);
}

#ifndef BUILD_NAGIOS
void nebmodule_event_callback(struct nm_event_execution_properties *properties) {
    auto ecb = reinterpret_cast<statusengine::EventCallback *>(properties->user_data);
    const bool workRemaining = ecb->Callback();
    if (!(sigshutdown || sigrestart)) {
        if (workRemaining) {
            statusengine::Nebmodule::Instance().RegisterEventCallbackNow(ecb);
        }
        else {
            statusengine::Nebmodule::Instance().RegisterEventCallback(ecb);
        }
    }
}
#else
void nebmodule_event_callback(statusengine::EventCallback *ecb) {
    // Nagios reschedules this itself, the event is recurring. There is no way to ask for
    // an earlier run, so a callback with work left over simply waits for the next one.
    (void)ecb->Callback();
}
#endif
