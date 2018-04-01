#ifndef NAGIOS_SERVICE_CHECK_DATA_H
#define NAGIOS_SERVICE_CHECK_DATA_H

#include "nebmodule.h"
#include "NagiosObject.h"

namespace statusengine {
	class NagiosServiceCheckData : public NagiosObject {
	public:
		NagiosServiceCheckData(const nebstruct_service_check_data *serviceCheckData);
	};
}

#endif // !NAGIOS_SERVICE_CHECK_DATA_H
