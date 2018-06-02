#pragma once

#include "NagiosObject.h"
#include "Nebmodule.h"

namespace statusengine {
    class NagiosProcessData : public NagiosObject {
      public:
        explicit NagiosProcessData(const nebstruct_process_data *processData);
    };
} // namespace statusengine
