#include "ContactNotificationDataCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosContactNotificationData.h"


namespace statusengine {
	ContactNotificationDataCallback::ContactNotificationDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_CONTACT_NOTIFICATION_DATA, se) {
	}

	void ContactNotificationDataCallback::Callback(int event_type, nebstruct_contact_notification_data *data) {
		auto myData = NagiosContactNotificationData(data);
		se->Gearman().SendMessage("statusngin_contactnotificationdata", myData.GetData().dump());
	}
}
