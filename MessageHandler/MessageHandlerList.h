#pragma once

#include <map>
#include <memory>
#include <vector>

#include "MessageHandler.h"
#include "MessageQueueHandler.h"

namespace statusengine {
    class Statusengine;
    class Configuration;

    class MessageHandlerList {
      public:
        MessageHandlerList(Statusengine *se, Configuration *cfg);
        ~MessageHandlerList();

        virtual bool Connect();

        virtual void FlushBulkQueue();
        virtual void Worker();

        virtual bool QueueExists(Queue queue);
        virtual std::shared_ptr<MessageQueueHandler> GetMessageQueueHandler(Queue queue);

      private:
        std::vector<std::shared_ptr<MessageHandler>> allHandlers;
        std::map<Queue, std::shared_ptr<MessageQueueHandler>> mqHandlers;
        Statusengine *se;
        unsigned long maxBulkSize;
        unsigned long globalBulkCounter;
    };
} // namespace statusengine
