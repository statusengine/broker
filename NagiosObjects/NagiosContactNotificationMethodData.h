#ifndef NAGIOS_CONTACTNOTIFICATIONMETHODDATA_H
#define NAGIOS_CONTACTNOTIFICATIONMETHODDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosContactNotificationMethodData : public NagiosObject {
	public:
		NagiosContactNotificationMethodData(const nebstruct_contact_notification_method_data *contactNotificationMethodData);
	};
}

#endif // !NAGIOS_CONTACTNOTIFICATIONMETHODDATA_H

