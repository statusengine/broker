#pragma once

#include "NagiosObject.h"
#include "Nebmodule.h"

namespace statusengine {
    class NagiosContactStatusData : public NagiosObject {
      public:
        explicit NagiosContactStatusData(const nebstruct_contact_status_data *contactStatusData);
    };
} // namespace statusengine
