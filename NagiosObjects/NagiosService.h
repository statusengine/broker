#ifndef NAGIOS_SERVICE_H
#define NAGIOS_SERVICE_H

#include "nebmodule.h"

#include "NagiosObject.h"

namespace statusengine {
	class NagiosService : public NagiosObject {
	public:
		NagiosService(const service *data);
	};

}

#endif // !NAGIOS_SERVICE_H
