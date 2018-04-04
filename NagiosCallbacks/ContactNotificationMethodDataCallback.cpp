#include "ContactNotificationMethodDataCallback.h"

#include "Statusengine.h"
#include "NagiosObjects/NagiosContactNotificationMethodData.h"


namespace statusengine {
	ContactNotificationMethodDataCallback::ContactNotificationMethodDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_CONTACT_NOTIFICATION_METHOD_DATA, se) {
	}

	void ContactNotificationMethodDataCallback::Callback(int event_type, nebstruct_contact_notification_method_data *data) {
		auto myData = NagiosContactNotificationMethodData(data);
		se->SendMessage("statusngin_contactnotificationmethod", myData.GetData().dump());
	}
}
