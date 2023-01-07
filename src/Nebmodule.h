#pragma once

#include <string>
#include <uchardet.h>

#include "INebmodule.h"

namespace statusengine {

    class Nebmodule : public INebmodule {
      public:
        explicit Nebmodule(Neb_NebmodulePtr handle, std::string args);
        virtual ~Nebmodule();

        int Init(nebmodule *handle, std::string args);

        int Deinit(int reason);

        int Callback(int event_type, void *data);

        bool RegisterCallback(NEBCallbackType cbType);

        void RegisterEventCallback(EventCallback *ecb);

        void ScheduleHostCheckDelay(host *temp_host, time_t delay);

        void ScheduleHostCheckFixed(host *temp_host, time_t fixed);

        void ScheduleServiceCheckDelay(service *temp_service, time_t delay);

        void ScheduleServiceCheckFixed(service *temp_service, time_t fixed);

        void DeleteDowntime(const char *hostname, const char *service_description, time_t start_time, time_t end_time, const char *comment);

        std::string EncodeString(char *data);

        Neb_NebmodulePtr GetNebNebmodulePtr(); 

      private:
        explicit Nebmodule() : se(nullptr), uc(nullptr) {}

        Statusengine *se;
        uchardet_t uc;
        Neb_NebmodulePtr handle;
    };
} // namespace statusengine

int nebmodule_callback(int event_type, void *data);
