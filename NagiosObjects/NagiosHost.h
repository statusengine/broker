#pragma once

#include "Nebmodule.h"

#include "NagiosObject.h"

namespace statusengine {
    class NagiosHost : public NagiosObject {
      public:
        explicit NagiosHost(const host *data);
    };

} // namespace statusengine
