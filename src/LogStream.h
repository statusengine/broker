#pragma once

#include <sstream>
#include <string>

#include "Nebmodule.h"

namespace statusengine {
    /**
     * Severity of a log message, ordered from least to most severe. The numbering is what
     * makes the threshold comparison in LogStream work, so keep it ascending.
     */
    enum class LogLevel { Info = 0, Warning = 1, Error = 2 };

    /**
     * Collects a message and writes it to the naemon log once a LogLevel is streamed in:
     *
     *     Log() << "something happened: " << value << LogLevel::Warning;
     *
     * The LogLevel both terminates the message and gives its severity. Messages below the
     * configured level are dropped.
     */
    class LogStream {
      public:
        // Starts at Info so that everything Configuration::Load() reports is visible: it
        // runs before SetLogLevel() can apply the configured level, and that startup
        // output is what tells you why a broker failed to come up.
        LogStream() : ss(), level(LogLevel::Info) {}

        ~LogStream() = default;

        LogStream(const LogStream &LogStream) = delete;
        LogStream(LogStream &&LogStream) = delete;
        LogStream &operator=(const LogStream &) = delete;

        /// Appends anything std::stringstream can format.
        template <typename T>
        LogStream &operator<<(const T &o) {
            ss << o;
            return *this;
        }

        /// Terminates the message and writes it out if it reaches the configured level.
        LogStream &operator<<(LogLevel messageLevel) {
            if (messageLevel >= level) {
                nm_log(NaemonLogLevel(messageLevel), "%s", ("Statusengine: " + ss.str()).c_str());
            }

            ss.str("");
            ss.clear();
            return *this;
        }

        void SetLogLevel(LogLevel ll) {
            level = ll;
        }

        LogLevel GetLogLevel() const {
            return level;
        }

      private:
        static int NaemonLogLevel(LogLevel messageLevel) {
            switch (messageLevel) {
                case LogLevel::Error:
                    return NSLOG_RUNTIME_ERROR;
                case LogLevel::Warning:
                    return NSLOG_RUNTIME_WARNING;
                case LogLevel::Info:
                    break;
            }
            return NSLOG_INFO_MESSAGE;
        }

        std::stringstream ss;
        LogLevel level;
    };
} // namespace statusengine
