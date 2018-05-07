#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosAcknowledgementData : public NagiosObject {
      public:
        explicit NagiosAcknowledgementData(const nebstruct_acknowledgement_data *acknowledgementData);
    };
} // namespace statusengine
