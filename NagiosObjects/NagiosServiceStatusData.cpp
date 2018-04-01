#include "NagiosServiceStatusData.h"

#include "NagiosService.h"


namespace statusengine {

	NagiosServiceStatusData::NagiosServiceStatusData(const nebstruct_service_status_data *serviceStatusData) {
		SetData<>("type", serviceStatusData->type);
		SetData<>("flags", serviceStatusData->flags);
		SetData<>("attr", serviceStatusData->attr);
		SetData<>("timestamp", serviceStatusData->timestamp.tv_sec);
		SetData<>("servicestatus", NagiosService(reinterpret_cast<service*>(serviceStatusData->object_ptr)).GetData());
	}
}
