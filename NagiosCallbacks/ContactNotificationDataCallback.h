#ifndef CONTACTNOTIFICATIONDATA_CALLBACK_H
#define CONTACTNOTIFICATIONDATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
	class ContactNotificationDataCallback : public NebmoduleCallback<nebstruct_contact_notification_data> {
	public:
		ContactNotificationDataCallback(Statusengine *se);

		virtual void Callback(int event_type, nebstruct_contact_notification_data *data);
	};
}

#endif // !CONTACTNOTIFICATIONDATA_CALLBACK_H

