#pragma once

#include <map>
#include <memory>
#include <vector>

#include "IStatusengine.h"
#include "IMessageHandler.h"
#include "Configuration.h"


namespace statusengine {

    class MessageHandlerList : public IMessageHandlerList {
      public:
        MessageHandlerList(IStatusengine &se, Configuration &cfg);

        ~MessageHandlerList() override {
            FlushBulkQueue();
        }

        void InitComplete() override {
            flushInProgress = false;
        }

        void FlushBulkQueue() override {
            if (globalBulkCounter > 0 && !flushInProgress) {
                flushInProgress = true;
                se.Log() << "Flush Bulk Queues" << LogLevel::Info;

                for (auto &handler : mqHandlers) {
                    handler.second->FlushBulkQueue();
                }
                globalBulkCounter = 0;
                flushInProgress = false;
            }
        }

        bool Connect() override {
            for (auto &handler : allHandlers) {
                if (!handler->Connect()) {
                    return false;
                }
            }
            return true;
        }

        std::shared_ptr<IMessageQueueHandler> GetMessageQueueHandler(Queue queue) override {
            return mqHandlers.at(queue);
        }

        bool QueueExists(Queue queue) override {
            return mqHandlers.find(queue) != mqHandlers.end();
        }

        void Worker() override {
            RunWorkers(allHandlers, maxWorkerMessagesPerInterval);
        }

        /**
         * Lets every handler drain its queue, until nobody has anything left or
         * maxMessages have been processed.
         *
         * A handler may ask for another round without having processed anything - the
         * gearman worker does exactly that when its socket would block. Such a handler
         * cannot advance the message counter, so counting messages alone does not bound
         * this loop, and it runs inside naemon's event loop. A round that made no progress
         * therefore ends it, and the next worker tick picks the work up again.
         *
         * Static and defined here so it can be exercised with fake handlers.
         */
        static void RunWorkers(std::vector<std::shared_ptr<IMessageHandler>> &handlers,
                               unsigned long maxMessages) {
            unsigned long counter = 0ul;
            bool moreMessages;
            do {
                moreMessages = false;
                const unsigned long before = counter;
                for (auto &handler : handlers) {
                    if (handler->Worker(counter)) {
                        moreMessages = true;
                    }
                }
                if (counter == before) {
                    break;
                }
            } while (moreMessages && (counter < maxMessages));
        }


      private:
        std::vector<std::shared_ptr<IMessageHandler>> allHandlers;
        std::map<Queue, std::shared_ptr<IMessageQueueHandler>> mqHandlers;
        IStatusengine &se;
        unsigned long maxBulkSize;
        unsigned long globalBulkCounter;
        bool flushInProgress;
        unsigned long maxWorkerMessagesPerInterval;
    };
} // namespace statusengine
