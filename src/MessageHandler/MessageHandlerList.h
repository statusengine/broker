#pragma once

#include <map>
#include <memory>
#include <vector>

#include "../IStatusengine.h"
#include "IMessageHandler.h"
#include "../Configuration.h"
#include "IBulkMessageCounter.h"


namespace statusengine {

    class MessageHandlerList : public IMessageHandlerList, public IBulkMessageCounter {
      public:
        MessageHandlerList(IStatusengine &se, Configuration &cfg);

        ~MessageHandlerList() override {
            FlushBulkQueue();
        }

        void InitComplete() override {
            flushInProgress = false;
        }

        void IncrementCounter() override {
            if (++globalBulkCounter >= maxBulkSize) {
                FlushBulkQueue();
            } 
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
            unsigned long counter = 0ul;
            bool moreMessages;
            do {
                moreMessages = false;
                for (auto &handler : allHandlers) {
                    if (handler->Worker(counter)) {
                        moreMessages = true;
                    }
                }
            } while (moreMessages && (counter < maxWorkerMessagesPerInterval));
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
