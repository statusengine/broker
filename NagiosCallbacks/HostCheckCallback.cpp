#include "HostCheckCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosHostCheckData.h"


namespace statusengine {
	HostCheckCallback::HostCheckCallback(Statusengine *se, bool hostchecks, bool ocsp) :
		NebmoduleCallback(NEBCALLBACK_HOST_CHECK_DATA, se),
		hostchecks(hostchecks), ocsp(ocsp) {
	}

	void HostCheckCallback::Callback(int event_type, nebstruct_host_check_data *data) {
		if (data->type == NEBTYPE_HOSTCHECK_PROCESSED) {
			auto checkData = NagiosHostCheckData(data);
			if (hostchecks) {
				se->SendMessage("statusngin_hostchecks", checkData.GetData().dump());
			}
			if (ocsp) {
				se->SendMessage("statusngin_ochp", checkData.GetData().dump());
			}
		}
	}
}
