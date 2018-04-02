#include "LogDataCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosLogData.h"


namespace statusengine {
	LogDataCallback::LogDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_LOG_DATA, se) {
	}

	void LogDataCallback::Callback(int event_type, nebstruct_log_data *data) {
		auto logData = NagiosLogData(data);
		se->Gearman().SendMessage("statusngin_logentries", logData.GetData().dump());
	}
}
