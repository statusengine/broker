#ifndef NAGIOS_EVENTHANDLERDATA_H
#define NAGIOS_EVENTHANDLERDATA_H

#include "nebmodule.h"
#include "NagiosObject.h"


namespace statusengine {
	class NagiosEventHandlerData : public NagiosObject {
	public:
		NagiosEventHandlerData(const nebstruct_event_handler_data *eventHandlerData);
	};
}

#endif // !NAGIOS_EVENTHANDLERDATA_H

