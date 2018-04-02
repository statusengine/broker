#include "AcknowledgementDataCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosAcknowledgementData.h"


namespace statusengine {
	AcknowledgementDataCallback::AcknowledgementDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_ACKNOWLEDGEMENT_DATA, se) {
	}

	void AcknowledgementDataCallback::Callback(int event_type, nebstruct_acknowledgement_data *data) {
		auto myData = NagiosAcknowledgementData(data);
		se->Gearman().SendMessage("statusngin_acknowledgements", myData.GetData().dump());
	}
}
