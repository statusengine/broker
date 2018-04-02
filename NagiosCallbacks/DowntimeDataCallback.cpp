#include "DowntimeDataCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosDowntimeData.h"


namespace statusengine {
	DowntimeDataCallback::DowntimeDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_DOWNTIME_DATA, se) {
	}

	void DowntimeDataCallback::Callback(int event_type, nebstruct_downtime_data *data) {
		auto myData = NagiosDowntimeData(data);
		se->Gearman().SendMessage("statusngin_downtimes", myData.GetData().dump());
	}
}
