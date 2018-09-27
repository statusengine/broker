#pragma once

#include "NebmoduleCallback.h"

#include "MessageHandler/MessageHandlerList.h"
#include "MessageHandler/MessageQueueHandler.h"

namespace statusengine {
    template <typename N, typename D, NEBCallbackType CBT, Queue queue>
    class StandardCallback : public NebmoduleCallback {
      public:
        StandardCallback(Statusengine *se) : NebmoduleCallback(CBT, se) {
            qHandler = se->GetMessageHandler()->GetMessageQueueHandler(queue);
        }

        virtual void Callback(int event_type, void *data) {
            auto nData = reinterpret_cast<N *>(data);
            D *dData = new D(nData);
            qHandler->SendMessage(dData);
        }

      protected:
        std::shared_ptr<MessageQueueHandler> qHandler;
    };
} // namespace statusengine
