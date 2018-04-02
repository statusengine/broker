#include "NotificationDataCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosNotificationData.h"


namespace statusengine {
	NotificationDataCallback::NotificationDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_NOTIFICATION_DATA, se) {
	}

	void NotificationDataCallback::Callback(int event_type, nebstruct_notification_data *data) {
		auto myData = NagiosNotificationData(data);
		se->Gearman().SendMessage("statusngin_notifications", myData.GetData().dump());
	}
}
