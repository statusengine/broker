#include "ContactNotificationMethodDataCallback.h"

#include "NagiosObjects/NagiosContactNotificationMethodData.h"
#include "Statusengine.h"

namespace statusengine {
    ContactNotificationMethodDataCallback::ContactNotificationMethodDataCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_CONTACT_NOTIFICATION_METHOD_DATA, se) {}

    void ContactNotificationMethodDataCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_contact_notification_method_data *>(vdata);

        auto myData = NagiosContactNotificationMethodData(data);
        se->SendMessage("statusngin_contactnotificationmethod", myData.ToString());
    }
} // namespace statusengine
