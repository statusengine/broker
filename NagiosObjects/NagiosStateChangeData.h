#ifndef NAGIOS_STATECHANGE_DATA_H
#define NAGIOS_STATECHANGE_DATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosStateChangeData : public NagiosObject {
      public:
        explicit NagiosStateChangeData(const nebstruct_statechange_data *serviceStatusData);
    };
} // namespace statusengine

#endif // !NAGIOS_STATECHANGE_DATA_H
