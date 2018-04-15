#include "ContactStatusDataCallback.h"

#include "NagiosObjects/NagiosContactStatusData.h"
#include "Statusengine.h"

namespace statusengine {
    ContactStatusDataCallback::ContactStatusDataCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_CONTACT_STATUS_DATA, se) {}

    void
    ContactStatusDataCallback::Callback(int event_type,
                                        nebstruct_contact_status_data *data) {
        auto myData = NagiosContactStatusData(data);
        se->SendMessage("statusngin_contactstatus", myData.GetData().dump());
    }
} // namespace statusengine
