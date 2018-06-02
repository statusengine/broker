#pragma once

#include <sstream>

#include "Nebmodule.h"

namespace statusengine {
    class Statusengine;

    enum class LogLevel { Info, Warning, Error };

    class LogStream {
      public:
        LogStream(Statusengine *se);
        ~LogStream();

        LogStream &operator<<(const char *o);
        LogStream &operator<<(const bool o);
        LogStream &operator<<(const short o);
        LogStream &operator<<(const unsigned short o);
        LogStream &operator<<(const int o);
        LogStream &operator<<(const unsigned int o);
        LogStream &operator<<(const long o);
        LogStream &operator<<(const unsigned long o);
        LogStream &operator<<(const long long o);
        LogStream &operator<<(const unsigned long long o);
        LogStream &operator<<(const float o);
        LogStream &operator<<(const double o);
        LogStream &operator<<(const long double o);
        LogStream &operator<<(const std::string o);
        LogStream &operator<<(const LogLevel o);

      private:
        LogStream(const LogStream &LogStream) = delete;
        LogStream(LogStream &&LogStream) = delete;
        LogStream &operator=(const LogStream &) = delete;

        std::stringstream *ss;

        Statusengine *se;
    };
} // namespace statusengine
