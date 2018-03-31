#ifndef NAGIOS_HOST_H
#define NAGIOS_HOST_H

#include "nebmodule.h"

#include "NagiosObject.h"

namespace statusengine {
	class NagiosHost : public NagiosObject {
	public:
		NagiosHost(const host *data);
	};

}

#endif // !NAGIOS_HOST_H
