#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosExternalCommandData : public NagiosObject {
      public:
        explicit NagiosExternalCommandData(const nebstruct_external_command_data *externalCommandData);
    };
} // namespace statusengine
