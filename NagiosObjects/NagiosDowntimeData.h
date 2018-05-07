#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosDowntimeData : public NagiosObject {
      public:
        explicit NagiosDowntimeData(const nebstruct_downtime_data *downtimeData);
    };
} // namespace statusengine
