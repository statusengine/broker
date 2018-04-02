#ifndef NAGIOS_PROCESS_DATA_H
#define NAGIOS_PROCESS_DATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosProcessData : public NagiosObject {
	public:
		NagiosProcessData(const nebstruct_process_data *processData);
	};
}

#endif // !NAGIOS_PROCESS_DATA_H
