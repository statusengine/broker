#pragma once

#include <map>
#include <memory>
#include <vector>

#include "MessageHandler.h"
#include "MessageQueueHandler.h"
#include "IMessageHandlerList.h"


namespace statusengine {
    class Statusengine;
    class Configuration;

    class MessageHandlerList : public IMessageHandlerList {
      public:
        MessageHandlerList(IStatusengine *se, Configuration *cfg);
        ~MessageHandlerList() override;

        void InitComplete() override;

        bool Connect() override;

        void FlushBulkQueue() override;
        void Worker() override;

        bool QueueExists(Queue queue) override;
        virtual std::shared_ptr<MessageQueueHandler> GetMessageQueueHandler(Queue queue);

      private:
        std::vector<std::shared_ptr<MessageHandler>> allHandlers;
        std::map<Queue, std::shared_ptr<MessageQueueHandler>> mqHandlers;
        IStatusengine *se;
        unsigned long maxBulkSize;
        unsigned long globalBulkCounter;
        bool flushInProgress;
        unsigned long maxWorkerMessagesPerInterval;
    };
} // namespace statusengine
