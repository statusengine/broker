#pragma once

#include <string>
#include <vector>

#include "IStatusengine.h"
#include "MessageHandler/IMessageHandler.h"

/// Everything nm_log() was called with, in order. Cleared by FakeStatusengine's constructor.
extern std::vector<std::string> capturedLogs;

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
        void ProcessMessage(WorkerQueue, json_object *) override {}

        unsigned long remaining;
        bool keepAsking;
        /// How often Worker() was entered - the guard against a spinning loop.
        unsigned long calls;
    };
} // namespace statusengine
