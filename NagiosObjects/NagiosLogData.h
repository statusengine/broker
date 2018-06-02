#pragma once

#include "NagiosObject.h"
#include "Nebmodule.h"

namespace statusengine {
    class NagiosLogData : public NagiosObject {
      public:
        explicit NagiosLogData(const nebstruct_log_data *logData);
    };
} // namespace statusengine
