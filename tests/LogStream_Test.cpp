#include <gtest/gtest.h>
#include <vector>

#include "LogStream.h"

namespace statusengine {

class LogStreamTest : public ::testing::TestWithParam<LogLevel> {};

TEST_P(LogStreamTest, LogLevel) {
    LogStream subject;
    auto level = GetParam();
    subject.SetLogLevel(level);
    ASSERT_EQ(subject.level, level);
}

INSTANTIATE_TEST_SUITE_P(LogStream, LogStreamTest,
                         testing::Values(LogLevel::Error, LogLevel::Info,
                                         LogLevel::Warning));

} // namespace statusengine
