#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch_all.hpp>

#include "LogStream.h"

void testMemory() {
    void *test = malloc(16);
    (void)test;
}


TEST_CASE("Log output") {
    statusengine::LogStream subject;
    REQUIRE_NOTHROW( subject.SetLogLevel(statusengine::LogLevel::Error));
}

TEST_CASE("Memory Test") {
    REQUIRE_NOTHROW( testMemory() );
}
