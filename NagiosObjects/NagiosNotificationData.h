#ifndef NAGIOS_NOTIFICATIONDATA_H
#define NAGIOS_NOTIFICATIONDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosNotificationData : public NagiosObject {
	public:
		NagiosNotificationData(const nebstruct_notification_data *notificationData);
	};
}

#endif // !NAGIOS_NOTIFICATIONDATA_H

