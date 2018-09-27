#pragma once

#include <memory>

#include "Nebmodule.h"

#include "MessageHandler/MessageQueueHandler.h"
#include "NebmoduleCallback.h"

namespace statusengine {
    class HostCheckCallback : public NebmoduleCallback {
      public:
        explicit HostCheckCallback(Statusengine *se);

        virtual void Callback(int event_type, void *vdata);

      private:
        bool hostchecks;
        bool ochp;

        std::shared_ptr<MessageQueueHandler> hostCheckHandler;
        std::shared_ptr<MessageQueueHandler> ochpHandler;
    };
} // namespace statusengine
