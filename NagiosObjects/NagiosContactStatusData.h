#ifndef NAGIOS_CONTACTSTATUSDATA_H
#define NAGIOS_CONTACTSTATUSDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosContactStatusData : public NagiosObject {
	public:
		NagiosContactStatusData(const nebstruct_contact_status_data *contactStatusData);
	};
}

#endif // !NAGIOS_CONTACTSTATUSDATA_H

