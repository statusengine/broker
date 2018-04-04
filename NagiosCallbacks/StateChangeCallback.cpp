#include "StateChangeCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosStateChangeData.h"


namespace statusengine {
	StateChangeCallback::StateChangeCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_STATE_CHANGE_DATA, se) {
	}

	void StateChangeCallback::Callback(int event_type, nebstruct_statechange_data *data) {
		auto statusData = NagiosStateChangeData(data);
		se->SendMessage("statusngin_statechanges", statusData.GetData().dump());
	}
}
