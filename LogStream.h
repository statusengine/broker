#pragma once

#include <sstream>

#include "Nebmodule.h"

namespace statusengine {
    enum class LogLevel { Info, Warning, Error };

    class LogStream {
      public:
        LogStream() : ss() {
            *this << "Logstream initalized" << LogLevel::Info;
        }

        ~LogStream() = default;

        LogStream &operator<<(const char *o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const bool o) {
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

        LogStream &operator<<(const std::string o) {
            ss << o;
            return *this;
        }

        LogStream &operator<<(const LogLevel o) {
            int logLevel;
            switch (o) {
                case LogLevel::Error:
                    logLevel = NSLOG_RUNTIME_ERROR;
                    break;
                case LogLevel::Warning:
                    logLevel = NSLOG_RUNTIME_WARNING;
                    break;
                default:
                    logLevel = NSLOG_INFO_MESSAGE;
                    break;
            }
            nm_log(logLevel, "%s", ("Statusengine: " + ss.str()).c_str());
            ss.str("");
            ss.clear();
            return *this;
        }

        LogStream(const LogStream &LogStream) = delete;
        LogStream(LogStream &&LogStream) = delete;
        LogStream &operator=(const LogStream &) = delete;

      private:
        std::stringstream ss;
    };
} // namespace statusengine
