#pragma once

#include "NagiosObject.h"
#include "Nebmodule.h"

namespace statusengine {
    class NagiosFlappingData : public NagiosObject {
      public:
        explicit NagiosFlappingData(const nebstruct_flapping_data *flappingData);
    };
} // namespace statusengine
