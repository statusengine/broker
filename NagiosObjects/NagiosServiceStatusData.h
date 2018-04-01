#ifndef NAGIOS_SERVICE_STATUS_DATA_H
#define NAGIOS_SERVICE_STATUS_DATA_H

#include "nebmodule.h"
#include "NagiosObject.h"

namespace statusengine {
	class NagiosServiceStatusData : public NagiosObject {
	public:
		NagiosServiceStatusData(const nebstruct_service_status_data *serviceStatusData);
	};
}

#endif // !NAGIOS_SERVICE_STATUS_DATA_H
