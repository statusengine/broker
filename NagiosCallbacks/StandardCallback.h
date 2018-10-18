#pragma once

#include "NebmoduleCallback.h"

#include "MessageHandler/MessageHandlerList.h"
#include "MessageHandler/MessageQueueHandler.h"
#include "Statusengine.h"

namespace statusengine {
    template <typename N, typename D, NEBCallbackType CBT, Queue queue>
    class StandardCallback : public NebmoduleCallback {
      public:
        explicit StandardCallback(Statusengine *se) : NebmoduleCallback(CBT, se) {
            qHandler = se->GetMessageHandler()->GetMessageQueueHandler(queue);
        }

        StandardCallback(StandardCallback &&other) noexcept
            : NebmoduleCallback::NebmoduleCallback(std::move(other)), qHandler(std::move(other.qHandler)) {}

        void Callback(int event_type, void *data) override {
            auto nData = reinterpret_cast<N *>(data);
            D dData(nData);
            qHandler->SendMessage(dData);
        }

      protected:
        std::shared_ptr<MessageQueueHandler> qHandler;
    };
} // namespace statusengine
