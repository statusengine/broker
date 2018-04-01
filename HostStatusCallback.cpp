#include "HostStatusCallback.h"

#include "Statusengine.h"
#include "LogStream.h"
#include "NagiosHostStatusData.h"

namespace statusengine {
	HostStatusCallback::HostStatusCallback(Statusengine *se) : se(se) {
	}

	void HostStatusCallback::Callback(int event_type, nebstruct_host_status_data *data) {
		auto statusData = NagiosHostStatusData(data);
		se->Gearman().SendMessage("statusngin_hoststatus", statusData.GetData().dump());
	}
}
