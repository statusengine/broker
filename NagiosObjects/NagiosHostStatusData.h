#ifndef NAGIOS_HOST_STATUS_DATA_H
#define NAGIOS_HOST_STATUS_DATA_H

#include "nebmodule.h"
#include "NagiosObject.h"

namespace statusengine {
	class NagiosHostStatusData : public NagiosObject {
	public:
		NagiosHostStatusData(const nebstruct_host_status_data *hostStatusData);
	};
}

#endif // !NAGIOS_HOST_STATUS_DATA_H
