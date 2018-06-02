#pragma once

#include "NagiosObject.h"
#include "Nebmodule.h"

namespace statusengine {
    class NagiosHostCheckData : public NagiosObject {
      public:
        explicit NagiosHostCheckData(const nebstruct_host_check_data *hostCheckData);
    };
} // namespace statusengine
