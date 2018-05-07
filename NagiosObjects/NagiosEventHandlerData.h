#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosEventHandlerData : public NagiosObject {
      public:
        explicit NagiosEventHandlerData(const nebstruct_event_handler_data *eventHandlerData);
    };
} // namespace statusengine
