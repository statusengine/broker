#include <cstring>
#include <ctime>

#include "EventCallback.h"
#include "Nebmodule.h"
#include "Statusengine.h"

void nebmodule_event_callback(struct nm_event_execution_properties *);

namespace statusengine {

void Nebmodule::RegisterEventCallback(EventCallback *ecb) {
    schedule_event(ecb->Interval(), nebmodule_event_callback, ecb);
}

void Nebmodule::ScheduleHostCheckDelay(host *temp_host, time_t delay) {
    schedule_next_host_check(temp_host, delay, CHECK_OPTION_NONE);
}

void Nebmodule::ScheduleHostCheckFixed(host *temp_host, time_t schedule_time) {
    time_t delay = schedule_time - std::time(nullptr);
    if (delay < 0) {
        delay = 0;
    }
    schedule_next_host_check(temp_host, delay, CHECK_OPTION_NONE);
}

void Nebmodule::ScheduleServiceCheckDelay(service *temp_service, time_t delay) {
    schedule_next_service_check(temp_service, delay, CHECK_OPTION_NONE);
}

void Nebmodule::ScheduleServiceCheckFixed(service *temp_service,
                                          time_t schedule_time) {
    time_t delay = schedule_time - std::time(nullptr);
    if (delay < 0) {
        delay = 0;
    }
    schedule_next_service_check(temp_service, delay, CHECK_OPTION_NONE);
}

void Nebmodule::DeleteDowntime(const char *hostname,
                               const char *service_description,
                               time_t start_time, time_t end_time,
                               const char *comment) {
    scheduled_downtime *temp_downtime;
    scheduled_downtime *next_downtime;
    void *downtime_cpy;
    objectlist *matches = NULL, *tmp_match = NULL;

    /* Do not allow deletion of everything - must have at least 1 filter on */
    if (hostname == NULL && service_description == NULL && start_time == 0 &&
        end_time == 0 && comment == NULL)
        return;

    for (temp_downtime = scheduled_downtime_list; temp_downtime != NULL;
         temp_downtime = next_downtime) {
        next_downtime = temp_downtime->next;
        if (start_time != 0 && temp_downtime->start_time != start_time) {
            continue;
        }
        if (end_time != 0 && temp_downtime->end_time != end_time) {
            continue;
        }
        if (comment != NULL &&
            std::strcmp(temp_downtime->comment, comment) != 0)
            continue;
        if (temp_downtime->type == HOST_DOWNTIME) {
            /* If service is specified, then do not delete the host downtime */
            if (service_description != NULL)
                continue;
            if (hostname != NULL &&
                std::strcmp(temp_downtime->host_name, hostname) != 0)
                continue;
        } else if (temp_downtime->type == SERVICE_DOWNTIME) {
            if (hostname != NULL &&
                std::strcmp(temp_downtime->host_name, hostname) != 0)
                continue;
            if (service_description != NULL &&
                std::strcmp(temp_downtime->service_description,
                            service_description) != 0)
                continue;
        }

        downtime_cpy = nm_malloc(sizeof(scheduled_downtime));
        std::memcpy(downtime_cpy, temp_downtime, sizeof(scheduled_downtime));
        prepend_object_to_objectlist(&matches, downtime_cpy);
        if (temp_downtime->type == HOST_DOWNTIME) {
            se->Log() << "Delete Host Downtime of Host '"
                      << temp_downtime->host_name
                      << "' with start time: " << temp_downtime->start_time
                      << " and end time: " << temp_downtime->end_time
                      << " and comment: '" << temp_downtime->comment << "'"
                      << LogLevel::Info;
        } else {
            se->Log() << "Delete Service Downtime of Host'"
                      << temp_downtime->host_name
                      << "' with service description '"
                      << temp_downtime->service_description
                      << "' with start time: " << temp_downtime->start_time
                      << " and end time: " << temp_downtime->end_time
                      << " and comment: '" << temp_downtime->comment << "'"
                      << LogLevel::Info;
        }
    }

    for (tmp_match = matches; tmp_match != NULL; tmp_match = tmp_match->next) {
        temp_downtime = (scheduled_downtime *)tmp_match->object_ptr;
        unschedule_downtime(temp_downtime->type, temp_downtime->downtime_id);
        nm_free(temp_downtime);
    }

    free_objectlist(&matches);
}
} // namespace statusengine

void nebmodule_event_callback(
    struct nm_event_execution_properties *properties) {
    auto ecb =
        reinterpret_cast<statusengine::EventCallback *>(properties->user_data);
    ecb->Callback();
    if (!(sigshutdown || sigrestart)) {
        schedule_event(ecb->Interval(), nebmodule_event_callback, ecb);
    }
}
