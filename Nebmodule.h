#pragma once

#include <string>

#ifndef BUILD_NAGIOS
extern "C" {
#include "naemon/naemon.h"
}
#else
#include <cstring>

#include "nagios/broker.h"
#include "nagios/comments.h"
#include "nagios/common.h"
#include "nagios/downtime.h"
#include "nagios/macros.h"
#include "nagios/nagios.h"
#include "nagios/nebcallbacks.h"
#include "nagios/nebmodules.h"
#include "nagios/nebstructs.h"

typedef int NEBCallbackType;
typedef nagios_comment comment;

inline void nm_log(long unsigned logLevel, const char *_, const char *message) {
    char *temp = nullptr;
    temp = strdup(message);
    write_to_all_logs(temp, logLevel);
    delete temp;W
}
#endif

namespace statusengine {
    class Statusengine;
    class EventCallback;

    class Nebmodule {
      public:
        static int Init(nebmodule *handle, std::string args);

        static int Deinit(int reason);

        static int Callback(int event_type, void *data);

        static bool RegisterCallback(NEBCallbackType cbType);

        static void RegisterEventCallback(EventCallback *ecb);

        static void ScheduleHostCheckDelay(host *temp_host, time_t delay);

        static void ScheduleHostCheckFixed(host *temp_host, time_t fixed);

        static void ScheduleServiceCheckDelay(service *temp_service, time_t delay);

        static void ScheduleServiceCheckFixed(service *temp_service, time_t fixed);

      private:
        static Statusengine *se;
    };
} // namespace statusengine

int nebmodule_callback(int event_type, void *data);

#ifndef BUILD_NAGIOS
void nebmodule_event_callback(struct nm_event_execution_properties *properties);
#else
void nebmodule_event_callback(statusengine::EventCallback *ecb);
#endif // BUILD_NAGIOS
