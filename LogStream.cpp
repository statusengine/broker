#include "LogStream.h"

namespace statusengine {
    LogStream::LogStream(Statusengine *se) : se(se) {
        ss = new std::stringstream();
        *this << "Logstream initalized" << LogLevel::Info;
    }

    LogStream::~LogStream() {
        delete ss;
    }

    LogStream &LogStream::operator<<(const char *o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const bool o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const short o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const unsigned short o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const int o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const unsigned int o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const long o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const unsigned long o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const long long o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const unsigned long long o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const float o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const double o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const long double o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const std::string o) {
        *ss << o;
        return *this;
    }

    LogStream &LogStream::operator<<(const LogLevel o) {
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
        nm_log(logLevel, "%s", ("Statusengine: " + ss->str()).c_str());
        delete ss;
        ss = new std::stringstream();
        return *this;
    }
} // namespace statusengine
