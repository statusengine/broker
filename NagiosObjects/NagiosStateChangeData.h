#ifndef NAGIOS_STATECHANGE_DATA_H
#define NAGIOS_STATECHANGE_DATA_H

#include "nebmodule.h"
#include "NagiosObject.h"

namespace statusengine {
	class NagiosStateChangeData : public NagiosObject {
	public:
		NagiosStateChangeData(const nebstruct_statechange_data *serviceStatusData);
	};
}

#endif // !NAGIOS_STATECHANGE_DATA_H
