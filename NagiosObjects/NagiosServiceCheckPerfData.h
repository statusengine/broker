#ifndef NAGIOS_SERVICE_CHECK_PERFDATA_H
#define NAGIOS_SERVICE_CHECK_PERFDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"

namespace statusengine {
	class NagiosServiceCheckPerfData : public NagiosObject {
	public:
		NagiosServiceCheckPerfData(const nebstruct_service_check_data *serviceCheckData);
	};
}

#endif // !NAGIOS_SERVICE_CHECK_PERFDATA_H
