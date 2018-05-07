#ifndef NAGIOS_FLAPPINGDATA_H
#define NAGIOS_FLAPPINGDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosFlappingData : public NagiosObject {
      public:
        explicit NagiosFlappingData(const nebstruct_flapping_data *flappingData);
    };
} // namespace statusengine

#endif // !NAGIOS_FLAPPINGDATA_H
