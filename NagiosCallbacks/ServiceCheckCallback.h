#pragma once

#include <memory>

#include "MessageHandler/IMessageQueueHandler.h"
#include "Nebmodule.h"
#include "NebmoduleCallback.h"

namespace statusengine {
    class ServiceCheckCallback : public NebmoduleCallback {
      public:
        explicit ServiceCheckCallback(Statusengine *se);
        ServiceCheckCallback(ServiceCheckCallback &&other) noexcept;

        void Callback(int event_type, void *vdata) override;

      private:
        bool servicechecks;
        bool ocsp;
        bool service_perfdata;
        std::shared_ptr<IMessageQueueHandler> serviceCheckHandler;
        std::shared_ptr<IMessageQueueHandler> ocspHandler;
        std::shared_ptr<IMessageQueueHandler> servicePerfHandler;
    };
} // namespace statusengine
