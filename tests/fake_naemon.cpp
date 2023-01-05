//#include<naemon/naemon.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using testing::MockFunction;

extern "C" {
    void nm_log(int, const char *, ...) {
    }
}
