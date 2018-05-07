#ifndef NAGIOS_HOST_STATUS_DATA_H
#define NAGIOS_HOST_STATUS_DATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosHostStatusData : public NagiosObject {
      public:
        explicit NagiosHostStatusData(const nebstruct_host_status_data *hostStatusData);
    };
} // namespace statusengine

#endif // !NAGIOS_HOST_STATUS_DATA_H
