#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosProgramStatusData : public NagiosObject {
      public:
        explicit NagiosProgramStatusData(const nebstruct_program_status_data *programmStatusData);
    };
} // namespace statusengine
