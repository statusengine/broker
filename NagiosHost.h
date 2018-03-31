#ifndef NAGIOS_HOST_H
#define NAGIOS_HOST_H

#include <string>
#include "vendor/json.hpp"

#include "nebmodule.h"

#include "NagiosObject.h"

using json = nlohmann::json;

namespace statusengine {
	class NagiosHost : public NagiosObject {
	public:
		NagiosHost(const host *nagiosHostData);
	};

}

#endif // !NAGIOS_HOST_H
