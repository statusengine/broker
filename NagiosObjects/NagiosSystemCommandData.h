#ifndef NAGIOS_SYSTEM_COMMAND_DATA_H
#define NAGIOS_SYSTEM_COMMAND_DATA_H

#include "nebmodule.h"
#include "NagiosObject.h"

namespace statusengine {
	class NagiosSystemCommandData : public NagiosObject {
	public:
		NagiosSystemCommandData(const nebstruct_system_command_data *systemCommandData);
	};
}

#endif // !NAGIOS_SYSTEM_COMMAND_DATA_H
