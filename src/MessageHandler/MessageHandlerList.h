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
         * How many rounds in a row may end without a processed message before the loop
         * gives up and lets the next tick try again.
         *
         * This cannot be 1. A handler may legitimately ask for another round without
         * having processed anything: the gearman worker does that on every single job,
         * because its GRAB_JOB response is still in flight when the socket is first read
         * and libgearman reports GEARMAN_IO_WAIT. Ending the loop on the first such round
         * therefore ends it after roughly one message per tick.
         *
         * Draining 110000 jobs from a real job server - with concurrent submitters and a
         * competing worker - never produced a run longer than two rounds, so this is
         * almost entirely headroom. It exists only to bound a handler that has stopped
         * making progress altogether, which would otherwise spin inside naemon's event
         * loop and stop the monitoring core from scheduling anything at all.
         */
        static constexpr unsigned maxRoundsWithoutProgress = 16;

        /**
         * Lets every handler drain its queue, until nobody has anything left, maxMessages
         * have been processed, or nobody has made progress for maxRoundsWithoutProgress
         * rounds.
         *
         * Static and defined here so it can be exercised with fake handlers.
         */
        static void RunWorkers(std::vector<std::shared_ptr<IMessageHandler>> &handlers,
                               unsigned long maxMessages) {
            unsigned long counter = 0ul;
            unsigned roundsWithoutProgress = 0u;
            bool moreMessages;
            do {
                moreMessages = false;
                const unsigned long before = counter;
                for (auto &handler : handlers) {
                    if (handler->Worker(counter)) {
                        moreMessages = true;
                    }
                }
                if (counter != before) {
                    roundsWithoutProgress = 0u;
                }
                else if (++roundsWithoutProgress >= maxRoundsWithoutProgress) {
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
