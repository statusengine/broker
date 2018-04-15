#ifndef NAGIOS_SERVICE_STATUS_DATA_H
#define NAGIOS_SERVICE_STATUS_DATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosServiceStatusData : public NagiosObject {
      public:
        NagiosServiceStatusData(
            const nebstruct_service_status_data *serviceStatusData);
    };
} // namespace statusengine

#endif // !NAGIOS_SERVICE_STATUS_DATA_H
