#pragma once

#include <memory>
#include <vector>

#include "Queue.h"
#include "NagiosObject.h"
#include "IMessageQueueHandler.h"
#include "MessageHandler.h"
#include "IStatusengine.h"

namespace statusengine  {
    class MessageHandlerList;

    class MessageQueueHandler : public IMessageQueueHandler {
      public:
        MessageQueueHandler(IStatusengine *se, MessageHandlerList *mhlist, unsigned long maxBulkSize,
                            unsigned long *globalBulkCounter, Queue queue,
                            std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> handlers, bool bulk);
        /**
         * SendMessage
         * @param JsonObjectContainer contains json object that will be deleted after sending the message
         */
        void SendMessage(NagiosObject &obj) override;
        void FlushBulkQueue() override;

      private:
        MessageHandlerList *mhlist;
        IStatusengine *se;

        Queue queue;
        std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> handlers;
        std::vector<NagiosObject *> bulkMessages;

        unsigned long maxBulkSize;
        unsigned long *globalBulkCounter;
        bool bulk;
    };
} // namespace statusengine
