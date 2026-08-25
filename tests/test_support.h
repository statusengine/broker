#pragma once

#include <string>
#include <vector>

#include "IStatusengine.h"

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
} // namespace statusengine
