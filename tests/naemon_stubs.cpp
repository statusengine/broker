// Minimal stand-ins for the naemon symbols the tested code references. The broker is a
// shared module that naemon resolves at load time, so a test binary has to provide them
// itself. Only the symbols actually pulled in by the tested translation units are needed.

#include "test_support.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

#include "Nebmodule.h"

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

// --- symbols referenced by NagiosObject.h -----------------------------------

namespace {
    nagios_macros testMacros;
}

extern "C" nagios_macros *get_global_macros(void) {
    return &testMacros;
}

extern "C" int get_raw_command_line_r(nagios_macros *, command *, char *cmd, char **full_command, int) {
    // naemon hands back a malloc'ed string that the caller frees.
    *full_command = (cmd == nullptr) ? nullptr : strdup(cmd);
    return 0;
}

extern "C" int clear_volatile_macros_r(nagios_macros *) {
    return 0;
}

extern "C" struct comment *find_service_comment(unsigned long) {
    return nullptr;
}

extern "C" struct comment *find_host_comment(unsigned long) {
    return nullptr;
}

extern "C" const char *get_program_version(void) {
    return "test";
}

namespace statusengine {
    // Defined here instead of linking Nebmodule.cpp, which would pull in the whole naemon
    // scheduling and downtime surface for no benefit: the encoder is the only part of
    // Nebmodule that NagiosObject uses.
    std::string Nebmodule::EncodeString(const char *inputData) {
        return encoder.ToUtf8(inputData);
    }
} // namespace statusengine
