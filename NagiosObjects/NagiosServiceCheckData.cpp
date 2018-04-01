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

		servicecheck["host_name"] = serviceCheckData->host_name;
		servicecheck["service_description"] = serviceCheckData->service_description;
		servicecheck["command_line"] = (raw_command != nullptr ? std::string(raw_command) : nullptr);
		servicecheck["command_name"] = (nag_service->check_command != nullptr ? std::string(nag_service->check_command) : nullptr);
		servicecheck["output"] = serviceCheckData->output;
		servicecheck["long_output"] = serviceCheckData->long_output;
		servicecheck["perf_data"] = serviceCheckData->perf_data;
		servicecheck["check_type"] = serviceCheckData->check_type;
		servicecheck["current_attempt"] = serviceCheckData->current_attempt;
		servicecheck["max_attempts"] = serviceCheckData->max_attempts;
		servicecheck["state_type"] = serviceCheckData->state_type;
		servicecheck["state"] = serviceCheckData->state;
		servicecheck["timeout"] = serviceCheckData->timeout;
		servicecheck["start_time"] = serviceCheckData->start_time.tv_sec;
		servicecheck["end_time"] = serviceCheckData->end_time.tv_sec;
		servicecheck["early_timeout"] = serviceCheckData->early_timeout;
		servicecheck["execution_time"] = serviceCheckData->execution_time;
		servicecheck["latency"] = serviceCheckData->latency;
		servicecheck["return_code"] = serviceCheckData->return_code;
		SetData<>("servicecheck", servicecheck);
	}
}
