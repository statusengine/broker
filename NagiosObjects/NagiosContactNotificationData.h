#ifndef NAGIOS_CONTACTNOTIFICATIONDATA_H
#define NAGIOS_CONTACTNOTIFICATIONDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosContactNotificationData : public NagiosObject {
	public:
		NagiosContactNotificationData(const nebstruct_contact_notification_data *contactNotificationData);
	};
}

#endif // !NAGIOS_CONTACTNOTIFICATIONDATA_H

