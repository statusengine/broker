#pragma once

#include <memory>

#include "MessageHandler/MessageQueueHandler.h"
#include "Nebmodule.h"
#include "NebmoduleCallback.h"

namespace statusengine {
    class ServiceCheckCallback : public NebmoduleCallback {
      public:
        explicit ServiceCheckCallback(Statusengine *se);

        virtual void Callback(int event_type, void *vdata);

      private:
        bool servicechecks;
        bool ocsp;
        bool service_perfdata;
        std::shared_ptr<MessageQueueHandler> serviceCheckHandler;
        std::shared_ptr<MessageQueueHandler> ocspHandler;
        std::shared_ptr<MessageQueueHandler> servicePerfHandler;
    };
} // namespace statusengine
