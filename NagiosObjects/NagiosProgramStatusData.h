#ifndef NAGIOS_PROGRAMSTATUSDATA_H
#define NAGIOS_PROGRAMSTATUSDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosProgramStatusData : public NagiosObject {
	public:
		NagiosProgramStatusData(const nebstruct_program_status_data *programmStatusData);
	};
}

#endif // !NAGIOS_PROGRAMSTATUSDATA_H

