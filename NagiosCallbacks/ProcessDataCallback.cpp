#include "ProcessDataCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosStateChangeData.h"


namespace statusengine {
	ProcessDataCallback::ProcessDataCallback(Statusengine *se, bool restart_data, bool process_data, bool object_data) :
		NebmoduleCallback(NEBCALLBACK_PROCESS_DATA, se),
		restart_data(restart_data),
		process_data(process_data),
		object_data(object_data) {
	}

	void ProcessDataCallback::Callback(int event_type, nebstruct_process_data *data) {
		if (restart_data && data->type == NEBTYPE_PROCESS_START) {
			json restartData = {
				{"object_type", NEBTYPE_PROCESS_RESTART }
			};
			se->Gearman().SendMessage("statusngin_core_restart", restartData.dump());
		}

		// TODO: Object Dump?



		//auto statusData = NagiosStateChangeData(data);
		//se->Gearman().SendMessage("statusngin_statechanges", statusData.GetData().dump());
	}
}
