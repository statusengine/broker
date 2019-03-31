#pragma once

#include <memory>

#include "Nebmodule.h"

#include "MessageHandler/IMessageQueueHandler.h"
#include "NebmoduleCallback.h"

namespace statusengine {
    class HostCheckCallback : public NebmoduleCallback {
      public:
        explicit HostCheckCallback(IStatusengine *se);
        HostCheckCallback(HostCheckCallback && other) noexcept;

        void Callback(int event_type, void *vdata) override;

      private:
        bool hostchecks;
        bool ochp;

        std::shared_ptr<IMessageQueueHandler> hostCheckHandler;
        std::shared_ptr<IMessageQueueHandler> ochpHandler;
    };
} // namespace statusengine
