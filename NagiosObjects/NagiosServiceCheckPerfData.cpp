#include "NagiosServiceCheckPerfData.h"

#include "NagiosService.h"


namespace statusengine {

	NagiosServiceCheckPerfData::NagiosServiceCheckPerfData(const nebstruct_service_check_data *serviceCheckData) {
		SetData<>("type", serviceCheckData->type);
		SetData<>("flags", serviceCheckData->flags);
		SetData<>("attr", serviceCheckData->attr);
		SetData<>("timestamp", serviceCheckData->timestamp.tv_sec);

		json servicecheck;

		servicecheck["host_name"] = serviceCheckData->host_name;
		servicecheck["service_description"] = serviceCheckData->service_description;
		servicecheck["perf_data"] = serviceCheckData->perf_data;
		servicecheck["start_time"] = serviceCheckData->start_time.tv_sec;
		SetData<>("servicecheck", servicecheck);
	}
}
