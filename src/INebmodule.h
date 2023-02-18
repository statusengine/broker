#pragma once

#include <memory>
#include <string>

extern "C" {
#include <naemon/naemon.h>
}

namespace statusengine {
class Statusengine;
class EventCallback;

typedef nebmodule *Neb_NebmodulePtr;

class INebmodule {
  public:
    virtual void Init() = 0;
    virtual int Callback(int event_type, void *data) = 0;
    virtual bool RegisterCallback(NEBCallbackType cbType) = 0;
    virtual void RegisterEventCallback(EventCallback *ecb) = 0;
    virtual void ScheduleHostCheckDelay(host *temp_host, time_t delay) = 0;
    virtual void ScheduleHostCheckFixed(host *temp_host, time_t fixed) = 0;
    virtual void ScheduleServiceCheckDelay(service *temp_service,
                                           time_t delay) = 0;
    virtual void ScheduleServiceCheckFixed(service *temp_service,
                                           time_t fixed) = 0;
    virtual void DeleteDowntime(const char *hostname,
                                const char *service_description,
                                time_t start_time, time_t end_time,
                                const char *comment) = 0;
    virtual void ProcessExternalCommand(char *cmd) = 0;
    virtual Neb_NebmodulePtr GetNebNebmodulePtr() = 0;
    virtual std::string EncodeString(char *data) = 0;
};

typedef std::shared_ptr<INebmodule> INebmodulePtr;
} // namespace statusengine
