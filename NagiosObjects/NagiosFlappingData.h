#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosFlappingData : public NagiosObject {
      public:
        explicit NagiosFlappingData(const nebstruct_flapping_data *flappingData);
    };
} // namespace statusengine
