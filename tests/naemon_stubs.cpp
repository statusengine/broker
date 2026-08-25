// Minimal stand-ins for the naemon symbols the tested code references. The broker is a
// shared module that naemon resolves at load time, so a test binary has to provide them
// itself. Only the symbols actually pulled in by the tested translation units are needed.

#include "test_support.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
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

// --- check_result handling, referenced by MessageHandler::ParseCheckResult -------------
//
// free_check_result() really frees, so that the benchmark and any future test see the same
// allocation pattern as production and sanitizers can still catch a mismatch.

extern "C" int init_check_result(check_result *cr) {
    std::memset(cr, 0, sizeof(*cr));
    return 0;
}

extern "C" int free_check_result(check_result *cr) {
    free(cr->host_name);
    free(cr->service_description);
    free(cr->output);
    free(cr->output_file);
    cr->host_name = nullptr;
    cr->service_description = nullptr;
    cr->output = nullptr;
    cr->output_file = nullptr;
    return 0;
}

/// Counts calls so a caller can assert the result actually reached naemon.
unsigned long processedCheckResults = 0;

extern "C" int process_check_result(check_result *) {
    ++processedCheckResults;
    return 0;
}

// --- reached from ProcessMessage's other command branches --------------------------------

extern "C" host *find_host(const char *) {
    return nullptr;
}

extern "C" service *find_service(const char *, const char *) {
    return nullptr;
}

extern "C" int process_external_command1(char *) {
    return 0;
}

namespace statusengine {
    void Nebmodule::ScheduleHostCheckFixed(host *, time_t) {}
    void Nebmodule::ScheduleServiceCheckFixed(service *, time_t) {}
    void Nebmodule::DeleteDowntime(const char *, const char *, time_t, time_t, const char *) {}
} // namespace statusengine
