#include "Nebmodule.h"

#include <ctime>
#include <string>

#include "EventCallback/EventCallback.h"
#include "Statusengine.h"

// This is required by naemon
extern "C" {
NEB_API_VERSION(CURRENT_NEB_API_VERSION);
}

namespace statusengine {
    Statusengine *Nebmodule::se = nullptr;

    int Nebmodule::Init(nebmodule *handle, std::string args) {
        se = new Statusengine(handle, std::move(args));
        return se->Init();
    }

    int Nebmodule::Deinit(int reason) {
        delete se;
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
        schedule_event(ecb->Interval(), nebmodule_event_callback, ecb);
#else
        time_t interval = static_cast<time_t>(ecb->Interval());
        schedule_new_event(EVENT_USER_FUNCTION, 1, std::time(0) + interval, 1, interval, nullptr, 1,
                           reinterpret_cast<void *>(nebmodule_event_callback), reinterpret_cast<void *>(ecb), 0);
#endif // BUILD_NAGIOS
    }

    void Nebmodule::ScheduleHostCheckDelay(host *temp_host, time_t delay) {
        time_t schedule_time = std::time(nullptr) + delay;
#ifndef BUILD_NAGIOS
        schedule_next_host_check(temp_host, delay, CHECK_OPTION_NONE);
#else
        schedule_host_check(temp_host, schedule_time, CHECK_OPTION_NONE);
#endif // BUILD_NAGIOS
    }

    void Nebmodule::ScheduleHostCheckFixed(host *temp_host, time_t schedule_time) {
        time_t delay = schedule_time - std::time(nullptr);
        if (schedule_time < 0) {
            delay = 0;
        }
#ifndef BUILD_NAGIOS
        schedule_next_host_check(temp_host, delay, CHECK_OPTION_NONE);
#else
        schedule_host_check(temp_host, schedule_time, CHECK_OPTION_NONE);
#endif // BUILD_NAGIOS
    }

    void Nebmodule::ScheduleServiceCheckDelay(service *temp_service, time_t delay) {
        time_t schedule_time = std::time(nullptr) + delay;
#ifndef BUILD_NAGIOS
        schedule_next_service_check(temp_service, delay, CHECK_OPTION_NONE);
#else
        schedule_service_check(temp_service, schedule_time, CHECK_OPTION_NONE);
#endif // BUILD_NAGIOS
    }

    void Nebmodule::ScheduleServiceCheckFixed(service *temp_service, time_t schedule_time) {
        time_t delay = schedule_time - std::time(nullptr);
        if (schedule_time < 0) {
            delay = 0;
        }
#ifndef BUILD_NAGIOS
        schedule_next_service_check(temp_service, delay, CHECK_OPTION_NONE);
#else
        schedule_service_check(temp_service, schedule_time, CHECK_OPTION_NONE);
#endif // BUILD_NAGIOS
    }
} // namespace statusengine

extern "C" int nebmodule_init(int flags, char *args, nebmodule *handle) {
    return statusengine::Nebmodule::Init(handle, std::string(args));
}

extern "C" int nebmodule_deinit(int flags, int reason) {
    return statusengine::Nebmodule::Deinit(reason);
}

int nebmodule_callback(int event_type, void *data) {
    return statusengine::Nebmodule::Callback(event_type, data);
}

#ifndef BUILD_NAGIOS
void nebmodule_event_callback(struct nm_event_execution_properties *properties) {
    auto ecb = reinterpret_cast<statusengine::EventCallback *>(properties->user_data);
    ecb->Callback();
    if (!(sigshutdown || sigrestart)) {
        statusengine::Nebmodule::RegisterEventCallback(ecb);
    }
}
#else
void nebmodule_event_callback(statusengine::EventCallback *ecb) {
    ecb->Callback();
}
#endif
