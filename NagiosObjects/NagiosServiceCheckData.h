#ifndef NAGIOS_SERVICE_CHECK_DATA_H
#define NAGIOS_SERVICE_CHECK_DATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosServiceCheckData : public NagiosObject {
      public:
        explicit NagiosServiceCheckData(
            const nebstruct_service_check_data *serviceCheckData);
    };
} // namespace statusengine

#endif // !NAGIOS_SERVICE_CHECK_DATA_H
