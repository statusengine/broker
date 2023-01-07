#pragma once

#include <sstream>
#include <string_view>

#include "Utility.h"
#include "INebmodule.h"

namespace statusengine {
    enum class LogLevel { Info, Warning, Error };

    class LogStream {
      FRIEND_TEST(LogStreamTest, LogLevel);
      public:
        LogStream() : ss(), level(LogLevel::Info) {
            *this << "Logstream initalized" << LogLevel::Info;
        }

        ~LogStream() = default;

        LogStream &operator<<(std::string_view o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const short o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const unsigned short o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const int o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const unsigned int o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const long o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const unsigned long o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const long long o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const unsigned long long o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const float o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const double o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const long double o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const LogLevel o) {
            int logLevel = -1;

            if (o == LogLevel::Error) {
                logLevel = NSLOG_RUNTIME_ERROR;
            }
            else if(o == LogLevel::Info && level == LogLevel::Info) {
                logLevel = NSLOG_INFO_MESSAGE;
            }
            else if(o == LogLevel::Warning && (level == LogLevel::Warning || level == LogLevel::Error)) {
                logLevel = NSLOG_RUNTIME_WARNING;
            }

            if (logLevel != -1) {
                nm_log(logLevel, "%s", ("Statusengine: " + ss.str()).c_str());
            }

            ss.str("");
            ss.clear();
            return *this;
        }

        LogStream(const LogStream &LogStream) = delete;
        LogStream(LogStream &&LogStream) = delete;
        LogStream &operator=(const LogStream &) = delete;

        void SetLogLevel(LogLevel ll) {
            level = ll;
        }

      private:
        std::stringstream ss;
        LogLevel level;
    };
} // namespace statusengine
