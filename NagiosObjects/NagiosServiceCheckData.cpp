#include "NagiosServiceCheckData.h"

#include "NagiosService.h"


namespace statusengine {

	NagiosServiceCheckData::NagiosServiceCheckData(const nebstruct_service_check_data *serviceCheckData) {
		SetData<>("type", serviceCheckData->type);
		SetData<>("flags", serviceCheckData->flags);
		SetData<>("attr", serviceCheckData->attr);
		SetData<>("timestamp", serviceCheckData->timestamp.tv_sec);

		service *nag_service = reinterpret_cast<service*>(serviceCheckData->object_ptr);

		char *raw_command = nullptr;
		get_raw_command_line_r(get_global_macros(), nag_service->check_command_ptr, nag_service->check_command, &raw_command, 0);

		json servicecheck;

		SetData<>("host_name", serviceCheckData->host_name, &servicecheck);
		SetData<>("service_description", serviceCheckData->service_description, &servicecheck);
		SetData<>("command_line", (raw_command != nullptr ? std::string(raw_command) : nullptr), &servicecheck);
		SetData<>("command_name", (nag_service->check_command != nullptr ? std::string(nag_service->check_command) : nullptr), &servicecheck);
		SetData<>("output", serviceCheckData->output, &servicecheck);
		SetData<>("long_output", serviceCheckData->long_output, &servicecheck);
		SetData<>("perf_data", serviceCheckData->perf_data, &servicecheck);
		SetData<>("check_type", serviceCheckData->check_type, &servicecheck);
		SetData<>("current_attempt", serviceCheckData->current_attempt, &servicecheck);
		SetData<>("max_attempts", serviceCheckData->max_attempts, &servicecheck);
		SetData<>("state_type", serviceCheckData->state_type, &servicecheck);
		SetData<>("state", serviceCheckData->state, &servicecheck);
		SetData<>("timeout", serviceCheckData->timeout, &servicecheck);
		SetData<>("start_time", serviceCheckData->start_time.tv_sec, &servicecheck);
		SetData<>("end_time", serviceCheckData->end_time.tv_sec, &servicecheck);
		SetData<>("early_timeout", serviceCheckData->early_timeout, &servicecheck);
		SetData<>("execution_time", serviceCheckData->execution_time, &servicecheck);
		SetData<>("latency", serviceCheckData->latency, &servicecheck);
		SetData<>("return_code", serviceCheckData->return_code, &servicecheck);

		SetData<>("servicecheck", servicecheck);
	}
}
