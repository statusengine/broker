#include "ContactNotificationDataCallback.h"

#include "NagiosObjects/NagiosContactNotificationData.h"
#include "Statusengine.h"

namespace statusengine {
    ContactNotificationDataCallback::ContactNotificationDataCallback(
        Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_CONTACT_NOTIFICATION_DATA, se) {}

    void ContactNotificationDataCallback::Callback(
        int event_type, nebstruct_contact_notification_data *data) {
        auto myData = NagiosContactNotificationData(data);
        se->SendMessage("statusngin_contactnotificationdata",
                        myData.ToString());
    }
} // namespace statusengine
