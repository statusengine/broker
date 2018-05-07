#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosStateChangeData : public NagiosObject {
      public:
        explicit NagiosStateChangeData(const nebstruct_statechange_data *serviceStatusData);
    };
} // namespace statusengine
