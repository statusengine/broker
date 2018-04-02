#ifndef NAGIOS_FLAPPINGDATA_H
#define NAGIOS_FLAPPINGDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosFlappingData : public NagiosObject {
	public:
		NagiosFlappingData(const nebstruct_flapping_data *flappingData);
	};
}

#endif // !NAGIOS_FLAPPINGDATA_H

