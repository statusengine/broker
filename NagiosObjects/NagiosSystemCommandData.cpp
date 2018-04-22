#include "NagiosSystemCommandData.h"

namespace statusengine {

    NagiosSystemCommandData::NagiosSystemCommandData(
        const nebstruct_system_command_data *systemCommandData) {
        SetData<>("type", systemCommandData->type);
        SetData<>("flags", systemCommandData->flags);
        SetData<>("attr", systemCommandData->attr);
        SetData<>("timestamp", systemCommandData->timestamp.tv_sec);

        json systemcommand;

        SetData<>("command_line", systemCommandData->command_line,
                  &systemcommand);
        SetData<>("output", EncodeString(systemCommandData->output), &systemcommand);
        SetData<>("long_output", EncodeString(systemCommandData->output), &systemcommand);
        SetData<>("start_time", systemCommandData->start_time.tv_sec,
                  &systemcommand);
        SetData<>("end_time", systemCommandData->end_time.tv_sec,
                  &systemcommand);
        SetData<>("timeout", systemCommandData->timeout, &systemcommand);
        SetData<>("early_timeout", systemCommandData->early_timeout,
                  &systemcommand);
        SetData<>("return_code", systemCommandData->return_code,
                  &systemcommand);
        SetData<>("execution_time", systemCommandData->execution_time,
                  &systemcommand);

        SetData<>("systemcommand", systemcommand);
    }
} // namespace statusengine
