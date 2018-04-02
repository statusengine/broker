#ifndef NAGIOS_LOG_DATA_H
#define NAGIOS_LOG_DATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosLogData : public NagiosObject {
	public:
		NagiosLogData(const nebstruct_log_data *logData);
	};
}

#endif // !NAGIOS_LOG_DATA_H
