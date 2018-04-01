#ifndef NAGIOS_HOST_CHECK_DATA_H
#define NAGIOS_HOST_CHECK_DATA_H

#include "nebmodule.h"
#include "NagiosObject.h"

namespace statusengine {
	class NagiosHostCheckData : public NagiosObject {
	public:
		NagiosHostCheckData(const nebstruct_host_check_data *hostCheckData);
	};
}

#endif // !NAGIOS_HOST_CHECK_DATA_H
