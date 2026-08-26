#pragma once

#include <chrono>
#include <ctime>
#include <map>
#include <memory>
#include <vector>

#include "IStatusengine.h"
#include "IMessageHandler.h"
#include "Configuration.h"


namespace statusengine {

    /// What one RunWorkers() slice managed to do.
    struct WorkerRunResult {
        /// Messages taken off the queues during this slice.
        unsigned long processed = 0ul;
        /// Stopped because the time budget ran out.
        bool budgetExhausted = false;
        /// Stopped because maxMessages was reached.
        bool messageLimitReached = false;

        /// True if the slice ended while the queues still had something to give.
        bool WorkRemaining() const {
            return budgetExhausted || messageLimitReached;
        }
    };

    /**
     * Reports that the worker keeps having to stop before the queues are empty, which
     * means the broker is handed messages faster than the monitoring core can apply them.
     *
     * A real backlog exhausts the budget on every single run, so at a 100ms budget one
     * line per run would be ten a second. Same shape as the gearman outage reporting: say
     * it when it starts, repeat it on an interval so it cannot be quietly forgotten, and
     * say when it is over.
     *
     * Takes the current time as an argument rather than reading the clock, so the interval
     * can be tested without waiting for it.
     */
    class OverloadReporter {
      public:
        explicit OverloadReporter(IStatusengine &se) : se(se), runs(0ul), messages(0ul), lastReport(0) {}

        static const time_t reportIntervalSeconds = 60;

        void Report(const WorkerRunResult &result, time_t now) {
            if (!result.WorkRemaining()) {
                if (runs > 0) {
                    se.Log() << "Worker queues are empty again, after " << runs
                             << " run(s) that had to stop early" << LogLevel::Warning;
                    runs = 0ul;
                    messages = 0ul;
                    lastReport = 0;
                }
                return;
            }

            ++runs;
            messages += result.processed;
            if (runs == 1ul || now - lastReport >= reportIntervalSeconds) {
                lastReport = now;
                se.Log() << "Worker stopped with messages still queued, having reached "
                         << (result.budgetExhausted ? "its time budget" : "its message limit")
                         << " (" << runs << " run(s), " << messages
                         << " message(s) so far). The queues are filling up faster than they can be applied."
                         << LogLevel::Warning;
            }
        }

      private:
        IStatusengine &se;
        unsigned long runs;
        unsigned long messages;
        time_t lastReport;
    };

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

        bool Worker() override {
#ifdef BUILD_NAGIOS
            // Nagios schedules the worker as a recurring one second event and offers no way
            // to ask for an earlier next slice, so a budget could only be paid for out of
            // throughput: whatever it cut short would wait a full second. Left unbounded
            // here, which is what nagios installations have always run with.
            const auto budget = std::chrono::milliseconds::zero();
#else
            const auto budget = maxWorkerRuntime;
#endif
            const auto result = RunWorkers(allHandlers, maxWorkerMessagesPerInterval, budget);
            overload.Report(result, std::time(nullptr));
            return result.WorkRemaining();
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
         * Lets every handler drain its queue, until nobody has anything left, the time
         * budget is spent, maxMessages have been processed, or nobody has made progress
         * for maxRoundsWithoutProgress rounds.
         *
         * This runs inside the monitoring core's event loop, so the budget is what keeps a
         * backlog from stopping the core: without it the loop only ends once the queues
         * are empty, and a single call was measured blocking naemon for 128 seconds while
         * it applied 100000 queued check results.
         *
         * The budget is checked between messages, and a message is not divisible - a bulk
         * message is one gearman job that has already been acknowledged, so abandoning it
         * half way would throw check results away. The guarantee is therefore "budget plus
         * the message in flight", not "budget".
         *
         * A budget of zero means unbounded.
         *
         * Static and defined here so it can be exercised with fake handlers, and templated
         * on the clock so the budget can be tested without sleeping.
         */
        template <typename Clock = std::chrono::steady_clock>
        static WorkerRunResult RunWorkers(std::vector<std::shared_ptr<IMessageHandler>> &handlers,
                                          unsigned long maxMessages, std::chrono::milliseconds budget) {
            WorkerRunResult result;
            unsigned roundsWithoutProgress = 0u;
            const bool bounded = budget > std::chrono::milliseconds::zero();
            const auto deadline = bounded ? Clock::now() + budget : typename Clock::time_point();
            bool moreMessages;
            do {
                moreMessages = false;
                const unsigned long before = result.processed;
                for (auto &handler : handlers) {
                    if (handler->Worker(result.processed)) {
                        moreMessages = true;
                    }
                }
                if (result.processed != before) {
                    roundsWithoutProgress = 0u;
                }
                else if (++roundsWithoutProgress >= maxRoundsWithoutProgress) {
                    break;
                }
                if (!moreMessages) {
                    break;
                }
                // Both of these mean work was left behind, which is what the caller
                // reschedules on. A round without progress does not - retrying that
                // immediately would just spin.
                //
                // Hence the "processed something" condition on both flags. A run that
                // spends its whole budget without completing a single message is not
                // behind, it is stuck: an unreachable job server makes libgearman report
                // GEARMAN_IO_WAIT round after round, each costing a 10ms poll, so the
                // budget is gone in ten rounds. Reporting that as work left over would
                // retry a broken connection ten times a second and, worse, blame the
                // wrong thing in the log - the handlers report a lost connection
                // themselves, and that is the message an operator needs.
                if (bounded && Clock::now() >= deadline) {
                    result.budgetExhausted = result.processed > 0ul;
                    break;
                }
                if (result.processed >= maxMessages) {
                    result.messageLimitReached = result.processed > 0ul;
                    break;
                }
            } while (true);
            return result;
        }


      private:
        std::vector<std::shared_ptr<IMessageHandler>> allHandlers;
        std::map<Queue, std::shared_ptr<IMessageQueueHandler>> mqHandlers;
        IStatusengine &se;
        unsigned long maxBulkSize;
        unsigned long globalBulkCounter;
        bool flushInProgress;
        unsigned long maxWorkerMessagesPerInterval;
        std::chrono::milliseconds maxWorkerRuntime;
        OverloadReporter overload;
    };
} // namespace statusengine
