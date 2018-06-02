#pragma once

#include "NagiosObject.h"
#include "Nebmodule.h"

namespace statusengine {
    class NagiosServiceCheckPerfData : public NagiosObject {
      public:
        explicit NagiosServiceCheckPerfData(const nebstruct_service_check_data *serviceCheckData);
    };
} // namespace statusengine
