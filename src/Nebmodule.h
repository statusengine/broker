#pragma once

#include <string>
#include <uchardet.h>

#include "INebmodule.h"

namespace statusengine {

    class Nebmodule : public INebmodule {
      public:
        explicit Nebmodule(Neb_NebmodulePtr handle, std::string args);
        virtual ~Nebmodule();
        
        void Init() override;

        int Callback(int event_type, void *data) override;

        bool RegisterCallback(NEBCallbackType cbType) override;

        void RegisterEventCallback(EventCallback *ecb) override;

        void ScheduleHostCheckDelay(host *temp_host, time_t delay) override;

        void ScheduleHostCheckFixed(host *temp_host, time_t fixed) override;

        void ScheduleServiceCheckDelay(service *temp_service, time_t delay) override;

        void ScheduleServiceCheckFixed(service *temp_service, time_t fixed) override;

        void DeleteDowntime(const char *hostname, const char *service_description, time_t start_time, time_t end_time, const char *comment) override;

        std::string EncodeString(char *data) override;

        Neb_NebmodulePtr GetNebNebmodulePtr() override; 

      private:
        explicit Nebmodule() : se(nullptr), uc(nullptr) {}

        Statusengine *se;
        uchardet_t uc;
        Neb_NebmodulePtr handle;
    };
} // namespace statusengine

int nebmodule_callback(int event_type, void *data);
