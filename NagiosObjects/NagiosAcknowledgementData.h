#ifndef NAGIOS_ACKNOWLEDGEMENTDATA_H
#define NAGIOS_ACKNOWLEDGEMENTDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosAcknowledgementData : public NagiosObject {
	public:
		NagiosAcknowledgementData(const nebstruct_acknowledgement_data *acknowledgementData);
	};
}

#endif // !NAGIOS_ACKNOWLEDGEMENTDATA_H

