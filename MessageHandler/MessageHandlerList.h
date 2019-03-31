#pragma once

#include <map>
#include <memory>
#include <vector>

#include "IStatusengine.h"
#include "IMessageHandler.h"

namespace statusengine {
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
        std::shared_ptr<IMessageQueueHandler> GetMessageQueueHandler(Queue queue) override;

      private:
        std::vector<std::shared_ptr<IMessageHandler>> allHandlers;
        std::map<Queue, std::shared_ptr<IMessageQueueHandler>> mqHandlers;
        IStatusengine *se;
        unsigned long maxBulkSize;
        unsigned long globalBulkCounter;
        bool flushInProgress;
        unsigned long maxWorkerMessagesPerInterval;
    };
} // namespace statusengine
