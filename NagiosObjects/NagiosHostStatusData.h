#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosHostStatusData : public NagiosObject {
      public:
        explicit NagiosHostStatusData(const nebstruct_host_status_data *hostStatusData);
    };
} // namespace statusengine
