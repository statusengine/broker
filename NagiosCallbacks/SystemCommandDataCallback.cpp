#include "SystemCommandDataCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosSystemCommandData.h"


namespace statusengine {
	SystemCommandDataCallback::SystemCommandDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_SYSTEM_COMMAND_DATA, se) {
	}

	void SystemCommandDataCallback::Callback(int event_type, nebstruct_system_command_data *data) {
		auto statusData = NagiosSystemCommandData(data);
		se->SendMessage("statusngin_systemcommands", statusData.GetData().dump());
	}
}
