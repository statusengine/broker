#include "NotificationDataCallback.h"

#include "NagiosObjects/NagiosNotificationData.h"
#include "Statusengine.h"

namespace statusengine {
    NotificationDataCallback::NotificationDataCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_NOTIFICATION_DATA, se) {}

    void NotificationDataCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_notification_data *>(vdata);

        auto myData = NagiosNotificationData(data);
        se->SendMessage("statusngin_notifications", myData.ToString());
    }
} // namespace statusengine
