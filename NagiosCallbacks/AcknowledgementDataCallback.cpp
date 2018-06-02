#include "AcknowledgementDataCallback.h"

#include "NagiosObjects/NagiosAcknowledgementData.h"
#include "Statusengine.h"

namespace statusengine {
    AcknowledgementDataCallback::AcknowledgementDataCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_ACKNOWLEDGEMENT_DATA, se) {}

    void AcknowledgementDataCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_acknowledgement_data *>(vdata);

        auto myData = NagiosAcknowledgementData(data);
        se->SendMessage("statusngin_acknowledgements", myData.ToString());
    }
} // namespace statusengine
