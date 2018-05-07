#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosServiceStatusData : public NagiosObject {
      public:
        explicit NagiosServiceStatusData(const nebstruct_service_status_data *serviceStatusData);
    };
} // namespace statusengine
