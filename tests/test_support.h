#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "IStatusengine.h"
#include "MessageHandler/IMessageHandler.h"

/// Everything nm_log() was called with, in order. Cleared by FakeStatusengine's constructor.
extern std::vector<std::string> capturedLogs;

/// How often the stubbed process_check_result() was called.
extern unsigned long processedCheckResults;

namespace statusengine {

    /**
     * IStatusengine that only provides logging. Enough for everything that just needs to
     * report configuration problems.
     */
    class FakeStatusengine : public IStatusengine {
      public:
        FakeStatusengine() : ls() {
            capturedLogs.clear();
            ls.SetLogLevel(LogLevel::Info);
        }

        LogStream &Log() override {
            return ls;
        }
        void FlushBulkQueue() override {}
        IMessageHandlerList *GetMessageHandler() const override {
            return nullptr;
        }
        void InitEventCallbacks() override {}
        time_t GetStartupScheduleMax() const override {
            return 0;
        }

        /// True if any log line contains the given text.
        static bool Logged(const std::string &needle) {
            for (const auto &line : capturedLogs) {
                if (line.find(needle) != std::string::npos) {
                    return true;
                }
            }
            return false;
        }

      private:
        LogStream ls;
    };

    /**
     * IMessageHandler whose Worker() behaviour is scripted, so the worker loop can be
     * driven through cases a real gearman or rabbitmq connection would be needed for.
     */
    class FakeMessageHandler : public IMessageHandler {
      public:
        /**
         * @param messages how many messages it reports as processed before running dry
         * @param keepAskingWhenDry whether it still claims more work once it is dry - what
         *        the gearman worker does on GEARMAN_IO_WAIT
         */
        FakeMessageHandler(unsigned long messages, bool keepAskingWhenDry)
            : remaining(messages), keepAsking(keepAskingWhenDry), calls(0) {}

        bool Worker(unsigned long &counter) override {
            ++calls;
            if (remaining > 0) {
                --remaining;
                ++counter;
                return true;
            }
            return keepAsking;
        }

        bool Connect() override {
            return true;
        }
        void SendMessage(Queue, const std::string &) override {}
        void ProcessMessage(WorkerQueue, const std::string &) override {}
        void ProcessMessage(WorkerQueue, yyjson_val *) override {}

        unsigned long remaining;
        bool keepAsking;
        /// How often Worker() was entered - the guard against a spinning loop.
        unsigned long calls;
    };

    /**
     * Clock that only moves when it is read, by a fixed step per reading. Lets the worker
     * loop's time budget be tested exactly and without sleeping: with a step of 10ms and a
     * budget of 50ms the loop is expected to stop after a known number of rounds.
     */
    struct FakeClock {
        using duration = std::chrono::steady_clock::duration;
        using rep = duration::rep;
        using period = duration::period;
        using time_point = std::chrono::time_point<FakeClock, duration>;
        static const bool is_steady = true;

        inline static std::chrono::milliseconds step{0};
        inline static unsigned long readings = 0ul;
        inline static time_point current{};

        /// Starts at zero again, advancing by perReading every time now() is called.
        static void Reset(std::chrono::milliseconds perReading) {
            step = perReading;
            readings = 0ul;
            current = time_point{};
        }

        static time_point now() {
            ++readings;
            const auto value = current;
            current += step;
            return value;
        }
    };

    /**
     * IMessageHandler shaped like the real gearman worker: every message costs one round
     * that reports "there is more" without processing anything, because the grab-job
     * response is still in flight, followed by a round that delivers it.
     *
     * Measured against a real job server this is not an edge case, it is what draining a
     * queue looks like: one GEARMAN_IO_WAIT per GEARMAN_SUCCESS, all the way down.
     */
    class FakeIoWaitHandler : public IMessageHandler {
      public:
        explicit FakeIoWaitHandler(unsigned long messages) : remaining(messages), waiting(true), calls(0) {}

        bool Worker(unsigned long &counter) override {
            ++calls;
            if (remaining == 0) {
                return false;
            }
            if (waiting) {
                // GEARMAN_IO_WAIT: more work is coming, but none of it is here yet.
                waiting = false;
                return true;
            }
            // GEARMAN_SUCCESS
            waiting = true;
            --remaining;
            ++counter;
            return true;
        }

        bool Connect() override {
            return true;
        }
        void SendMessage(Queue, const std::string &) override {}
        void ProcessMessage(WorkerQueue, const std::string &) override {}
        void ProcessMessage(WorkerQueue, yyjson_val *) override {}

        unsigned long remaining;
        bool waiting;
        unsigned long calls;
    };
} // namespace statusengine
