#include "AcknowledgementDataCallback.h"

#include "NagiosObjects/NagiosAcknowledgementData.h"
#include "Statusengine.h"

namespace statusengine {
    AcknowledgementDataCallback::AcknowledgementDataCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_ACKNOWLEDGEMENT_DATA, se) {}

    void AcknowledgementDataCallback::Callback(
        int event_type, nebstruct_acknowledgement_data *data) {
        auto myData = NagiosAcknowledgementData(data);
        se->SendMessage("statusngin_acknowledgements", myData.GetData().dump());
    }
} // namespace statusengine
