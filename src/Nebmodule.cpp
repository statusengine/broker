#include "Nebmodule.h"

#include <ctime>
#include <string>
#include <cstring>
#include <iconv.h>
#include <memory>

#include "EventCallback.h"
#include "Statusengine.h"
#include "Exceptions.h"

// This is required by naemon
extern "C" {
NEB_API_VERSION(CURRENT_NEB_API_VERSION)
}

namespace statusengine {

    Nebmodule::Nebmodule(Neb_NebmodulePtr handle, std::string args) : handle(handle) {
        se = new Statusengine(*this, std::move(args));
        uc = uchardet_new();
        se->Init();
    }

    Nebmodule::~Nebmodule() {
        delete se;
        uchardet_delete(uc);
        uc = nullptr;
    }

    int Nebmodule::Callback(int event_type, void *data) {
        return se->Callback(event_type, data);
    }

    bool Nebmodule::RegisterCallback(NEBCallbackType cbType) {
        int result = neb_register_callback(cbType, handle, 0, nebmodule_callback);

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
        schedule_event(ecb->Interval(), nebmodule_event_callback, ecb);
#else
        time_t interval = static_cast<time_t>(ecb->Interval());
        schedule_new_event(EVENT_USER_FUNCTION, 1, std::time(0) + interval, 1, interval, nullptr, 1,
                           reinterpret_cast<void *>(nebmodule_event_callback), reinterpret_cast<void *>(ecb), 0);
#endif // BUILD_NAGIOS
    }

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

    std::string Nebmodule::EncodeString(char *inputData) {
        if(inputData == nullptr) {
            return std::string();
        }
        auto lendata = std::strlen(inputData); // we can't use strnlen here, we don't have any idea of the length here...
        uchardet_handle_data(uc, inputData, lendata); //TODO error handling
        uchardet_data_end(uc);
        auto charset = uchardet_get_charset(uc);
        uchardet_reset(uc);

        if(std::strcmp(charset, "UTF-8")) {
            // We don't have to convert it, if it is already UTF-8
            return std::string(inputData, lendata);
        }
        auto outputDataLength = lendata*4;
        char *outputData = new char[outputDataLength]; // utf-8 possibly needs up to 4 bytes for a single character :/
        
        auto cd = iconv_open("UTF-8", charset);
        auto outputLength = iconv(cd, &inputData, &lendata, &outputData, &outputDataLength);
        std::string result(outputData, outputLength);
        iconv_close(cd);

        delete [] outputData;
        delete [] charset;

        return result;
    }

    Neb_NebmodulePtr Nebmodule::GetNebNebmodulePtr() {
        return handle;
    }
} // namespace statusengine

static statusengine::INebmodulePtr instance;

extern "C" int nebmodule_init(int, char *args, nebmodule *handle) {
    try {
        instance = std::make_shared<statusengine::Nebmodule>(handle, std::string(args));
        return 0;
    } catch (const statusengine::StatusengineException &e) {
        return 1;
    }
}

extern "C" int nebmodule_deinit(int, int) {
    try {
        instance.reset();
        return 0;
    } catch (const statusengine::StatusengineException &e) {
        return 1;
    }
}

int nebmodule_callback(int event_type, void *data) {
    return instance->Callback(event_type, data);
}

#ifndef BUILD_NAGIOS
void nebmodule_event_callback(struct nm_event_execution_properties *properties) {
    auto ecb = reinterpret_cast<statusengine::EventCallback *>(properties->user_data);
    ecb->Callback();
    if (!(sigshutdown || sigrestart)) {
        instance->RegisterEventCallback(ecb);
    }
}
#else
void nebmodule_event_callback(statusengine::EventCallback *ecb) {
    ecb->Callback();
}
#endif
