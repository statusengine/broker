#ifndef NAGIOS_EXTERNALCOMMANDDATA_H
#define NAGIOS_EXTERNALCOMMANDDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosExternalCommandData : public NagiosObject {
	public:
		NagiosExternalCommandData(const nebstruct_external_command_data *externalCommandData);
	};
}

#endif // !NAGIOS_EXTERNALCOMMANDDATA_H

