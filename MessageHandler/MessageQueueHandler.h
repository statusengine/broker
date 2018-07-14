#pragma once

#include <memory>
#include <vector>

#include "Configuration/Queue.h"
#include "MessageHandler.h"

namespace statusengine {
    class MessageHandlerList;
    class Statusengine;

    class MessageQueueHandler {
      public:
        MessageQueueHandler(Statusengine *se, MessageHandlerList *mhlist, unsigned long maxBulkSize,
                            unsigned long *globalBulkCounter, Queue queue,
                            std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> handlers);
        void SendMessage(const std::string &message);
        void SendBulkMessage(std::string message);
        void FlushBulkQueue();
        bool Connect();

      private:
        MessageHandlerList *mhlist;
        Statusengine *se;

        Queue queue;
        std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> handlers;
        std::vector<std::string> bulkMessages;

        unsigned long maxBulkSize;
        unsigned long *globalBulkCounter;
    };
} // namespace statusengine
