#pragma once

#include "NagiosObject.h"
#include "Nebmodule.h"

namespace statusengine {
    class NagiosProgramStatusData : public NagiosObject {
      public:
        explicit NagiosProgramStatusData(const nebstruct_program_status_data *programmStatusData);
    };
} // namespace statusengine
