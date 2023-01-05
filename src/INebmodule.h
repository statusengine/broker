#pragma once

#include <string>
#include <memory>

#ifndef BUILD_NAGIOS
extern "C"
{
#include <naemon/naemon.h>
}
#else
#include <cstring>

#include <nagios/broker.h>
#include <nagios/comments.h>
#include <nagios/common.h>
#include <nagios/downtime.h>
#include <nagios/macros.h>
#include <nagios/nagios.h>
#include <nagios/nebcallbacks.h>
#include <nagios/nebmodules.h>
#include <nagios/nebstructs.h>

typedef int NEBCallbackType;
typedef nagios_comment comment;

void nm_log(long unsigned logLevel, const char *, const char *message);
#endif

namespace statusengine
{
    class Statusengine;
    class EventCallback;

    typedef nebmodule* Neb_NebmodulePtr;

    class INebmodule
    {
    public:
        virtual int Callback(int event_type, void *data) = 0;
        virtual bool RegisterCallback(NEBCallbackType cbType) = 0;
        virtual void RegisterEventCallback(EventCallback *ecb) = 0;
        virtual void ScheduleHostCheckDelay(host *temp_host, time_t delay) = 0;
        virtual void ScheduleHostCheckFixed(host *temp_host, time_t fixed) = 0;
        virtual void ScheduleServiceCheckDelay(service *temp_service, time_t delay) = 0;
        virtual void ScheduleServiceCheckFixed(service *temp_service, time_t fixed) = 0;
        virtual void DeleteDowntime(const char *hostname, const char *service_description, time_t start_time, time_t end_time, const char *comment) = 0;
        virtual Neb_NebmodulePtr GetNebNebmodulePtr() = 0;
        virtual std::string EncodeString(char *data) = 0;
    };

    typedef std::shared_ptr<INebmodule> INebmodulePtr;
}
