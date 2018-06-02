#pragma once

#include "Nebmodule.h"

#include "NagiosObject.h"

namespace statusengine {
    class NagiosService : public NagiosObject {
      public:
        explicit NagiosService(const service *data);
    };

} // namespace statusengine
