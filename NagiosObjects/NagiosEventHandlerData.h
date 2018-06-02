#pragma once

#include "NagiosObject.h"
#include "Nebmodule.h"

namespace statusengine {
    class NagiosEventHandlerData : public NagiosObject {
      public:
        explicit NagiosEventHandlerData(const nebstruct_event_handler_data *eventHandlerData);
    };
} // namespace statusengine
