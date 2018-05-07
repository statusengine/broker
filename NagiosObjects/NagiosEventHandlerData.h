#ifndef NAGIOS_EVENTHANDLERDATA_H
#define NAGIOS_EVENTHANDLERDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosEventHandlerData : public NagiosObject {
      public:
        explicit NagiosEventHandlerData(const nebstruct_event_handler_data *eventHandlerData);
    };
} // namespace statusengine

#endif // !NAGIOS_EVENTHANDLERDATA_H
