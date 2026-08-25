#include <doctest/doctest.h>

#include "test_support.h"

#include "LogStream.h"

using statusengine::LogLevel;
using statusengine::LogStream;

namespace {
    /// Streams one message of each severity and reports which ones were written out.
    struct Emitted {
        bool info;
        bool warning;
        bool error;
    };

    Emitted EmitAll(LogLevel configured) {
        LogStream ls;
        ls.SetLogLevel(configured);

        capturedLogs.clear();
        ls << "an info message" << LogLevel::Info;
        bool info = !capturedLogs.empty();

        capturedLogs.clear();
        ls << "a warning message" << LogLevel::Warning;
        bool warning = !capturedLogs.empty();

        capturedLogs.clear();
        ls << "an error message" << LogLevel::Error;
        bool error = !capturedLogs.empty();

        return Emitted{info, warning, error};
    }
} // namespace

TEST_CASE("Info level emits everything") {
    auto e = EmitAll(LogLevel::Info);
    CHECK(e.info);
    CHECK(e.warning); // used to be dropped: the most verbose level hid all warnings
    CHECK(e.error);
}

TEST_CASE("Warning level drops info only") {
    auto e = EmitAll(LogLevel::Warning);
    CHECK_FALSE(e.info);
    CHECK(e.warning);
    CHECK(e.error);
}

TEST_CASE("Error level drops info and warning") {
    auto e = EmitAll(LogLevel::Error);
    CHECK_FALSE(e.info);
    CHECK_FALSE(e.warning); // used to be emitted even at the strictest level
    CHECK(e.error);
}

TEST_CASE("LogStream starts at info level") {
    // Startup diagnostics are logged before the configured level is known, so the initial
    // level has to be the most verbose one or they would be lost.
    LogStream ls;
    CHECK(ls.GetLogLevel() == LogLevel::Info);
}

TEST_CASE("LogStream prefixes messages and formats mixed types") {
    LogStream ls;
    ls.SetLogLevel(LogLevel::Info);
    capturedLogs.clear();

    ls << "host " << std::string("localhost") << " had " << 3 << " attempts, latency " << 1.5
       << ", flapping " << true << LogLevel::Info;

    REQUIRE(capturedLogs.size() == 1);
    CHECK(capturedLogs[0] == "Statusengine: host localhost had 3 attempts, latency 1.5, flapping 1");
}

TEST_CASE("The buffer is cleared between messages") {
    LogStream ls;
    ls.SetLogLevel(LogLevel::Info);

    capturedLogs.clear();
    ls << "first" << LogLevel::Info;
    ls << "second" << LogLevel::Info;

    REQUIRE(capturedLogs.size() == 2);
    CHECK(capturedLogs[0] == "Statusengine: first");
    CHECK(capturedLogs[1] == "Statusengine: second");
}

TEST_CASE("A dropped message does not leak into the next one") {
    LogStream ls;
    ls.SetLogLevel(LogLevel::Error);

    capturedLogs.clear();
    ls << "suppressed info" << LogLevel::Info;
    ls << "real error" << LogLevel::Error;

    REQUIRE(capturedLogs.size() == 1);
    CHECK(capturedLogs[0] == "Statusengine: real error");
}
