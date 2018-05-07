#include "NagiosHostCheckData.h"

#include "NagiosService.h"

namespace statusengine {

    NagiosHostCheckData::NagiosHostCheckData(const nebstruct_host_check_data *hostCheckData) {
        SetData<>("type", hostCheckData->type);
        SetData<>("flags", hostCheckData->flags);
        SetData<>("attr", hostCheckData->attr);
        SetData<>("timestamp", hostCheckData->timestamp.tv_sec);

        host *nag_host = reinterpret_cast<host *>(hostCheckData->object_ptr);

        char *raw_command = nullptr;
        get_raw_command_line_r(get_global_macros(), nag_host->check_command_ptr, nag_host->check_command, &raw_command,
                               0);

        NagiosObject hostcheck;

        hostcheck.SetData<>("host_name", hostCheckData->host_name);
        hostcheck.SetData<>("command_line", raw_command != nullptr ? raw_command : nullptr);
        hostcheck.SetData<>("command_name", nag_host->check_command != nullptr ? nag_host->check_command : nullptr);
        hostcheck.SetData<>("output", EncodeString(hostCheckData->output));
        hostcheck.SetData<>("long_output", EncodeString(hostCheckData->long_output));
        hostcheck.SetData<>("perf_data", EncodeString(hostCheckData->perf_data));
        hostcheck.SetData<>("check_type", hostCheckData->check_type);
        hostcheck.SetData<>("current_attempt", hostCheckData->current_attempt);
        hostcheck.SetData<>("max_attempts", hostCheckData->max_attempts);
        hostcheck.SetData<>("state_type", hostCheckData->state_type);
        hostcheck.SetData<>("state", hostCheckData->state);
        hostcheck.SetData<>("timeout", hostCheckData->timeout);
        hostcheck.SetData<>("start_time", hostCheckData->start_time.tv_sec);
        hostcheck.SetData<>("end_time", hostCheckData->end_time.tv_sec);
        hostcheck.SetData<>("early_timeout", hostCheckData->early_timeout);
        hostcheck.SetData<>("execution_time", hostCheckData->execution_time);
        hostcheck.SetData<>("latency", hostCheckData->latency);
        hostcheck.SetData<>("return_code", hostCheckData->return_code);

        SetData<>("hostcheck", &hostcheck);
    }
} // namespace statusengine
