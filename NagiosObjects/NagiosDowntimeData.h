#ifndef NAGIOS_DOWNTIMEDATA_H
#define NAGIOS_DOWNTIMEDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosDowntimeData : public NagiosObject {
	public:
		NagiosDowntimeData(const nebstruct_downtime_data *downtimeData);
	};
}

#endif // !NAGIOS_DOWNTIMEDATA_H

