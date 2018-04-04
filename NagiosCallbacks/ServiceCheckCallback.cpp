#include "ServiceCheckCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosServiceCheckData.h"
#include "NagiosObjects/NagiosServiceCheckPerfData.h"


namespace statusengine {
	ServiceCheckCallback::ServiceCheckCallback(Statusengine *se, bool servicechecks, bool ocsp, bool service_perfdata) : 
		NebmoduleCallback(NEBCALLBACK_SERVICE_CHECK_DATA, se),
		servicechecks(servicechecks), ocsp(ocsp), service_perfdata(service_perfdata) {
	}

	void ServiceCheckCallback::Callback(int event_type, nebstruct_service_check_data *data) {
		if (data->type == NEBTYPE_SERVICECHECK_PROCESSED) {
			if (servicechecks || ocsp) {
				auto checkData = NagiosServiceCheckData(data);
				if (servicechecks) {
					se->SendMessage("statusngin_servicechecks", checkData.GetData().dump());
				}
				if (ocsp) {
					se->SendMessage("statusngin_ocsp", checkData.GetData().dump());
				}
			}
			if (service_perfdata) {
				auto checkPerfData = NagiosServiceCheckPerfData(data);
				se->SendMessage("statusngin_service_perfdata", checkPerfData.GetData().dump());
			}
		}
	}
}
