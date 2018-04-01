#include "ServiceStatusCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosServiceStatusData.h"


namespace statusengine {
	ServiceStatusCallback::ServiceStatusCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_SERVICE_STATUS_DATA, se) {
	}

	void ServiceStatusCallback::Callback(int event_type, nebstruct_service_status_data *data) {
		auto statusData = NagiosServiceStatusData(data);
		se->Gearman().SendMessage("statusngin_servicestatus", statusData.GetData().dump());
	}
}
