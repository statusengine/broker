// Minimal stand-ins for the naemon symbols the tested code references. The broker is a
// shared module that naemon resolves at load time, so a test binary has to provide them
// itself. Only the symbols actually pulled in by the tested translation units are needed.

#include "test_support.h"

#include <cstdarg>
#include <cstdio>
#include <vector>

std::vector<std::string> capturedLogs;

// Signature has to match naemon/logging.h exactly.
extern "C" void nm_log(int, const char *fmt, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, fmt);
    std::vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    capturedLogs.emplace_back(buffer);
}
