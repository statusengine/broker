#pragma once

#include <memory>
#include <vector>

#include "Configuration/Queue.h"
#include "NagiosObjects/NagiosObject.h"
#include "MessageHandler.h"

namespace statusengine {
    class MessageHandlerList;
    class Statusengine;

    class MessageQueueHandler {
      public:
        MessageQueueHandler(Statusengine *se, MessageHandlerList *mhlist, unsigned long maxBulkSize,
                            unsigned long *globalBulkCounter, Queue queue,
                            std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> handlers, bool bulk);
        /**
         * SendMessage
         * @param JsonObjectContainer contains json object that will be deleted after sending the message
         */
        void SendMessage(NagiosObject *obj);
        void FlushBulkQueue();
        bool Connect();

      private:
        MessageHandlerList *mhlist;
        Statusengine *se;

        Queue queue;
        std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> handlers;
        std::vector<NagiosObject *> bulkMessages;

        unsigned long maxBulkSize;
        unsigned long *globalBulkCounter;
        bool bulk;
    };
} // namespace statusengine
