#include "ContactStatusDataCallback.h"

#include "NagiosObjects/NagiosContactStatusData.h"
#include "Statusengine.h"

namespace statusengine {
    ContactStatusDataCallback::ContactStatusDataCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_CONTACT_STATUS_DATA, se) {}

    void ContactStatusDataCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_contact_status_data *>(vdata);

        auto myData = NagiosContactStatusData(data);
        se->SendMessage("statusngin_contactstatus", myData.ToString());
    }
} // namespace statusengine
