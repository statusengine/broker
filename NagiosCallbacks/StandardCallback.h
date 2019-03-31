#pragma once

#include "NebmoduleCallback.h"

#include "MessageHandler/IMessageQueueHandler.h"
#include "IStatusengine.h"

namespace statusengine {
    template <typename N, typename D, NEBCallbackType CBT, Queue queue>
    class StandardCallback : public NebmoduleCallback {
      public:
        explicit StandardCallback(IStatusengine *se) : NebmoduleCallback(CBT, se) {
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
        std::shared_ptr<IMessageQueueHandler> qHandler;
    };
} // namespace statusengine
