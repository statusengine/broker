#ifndef NAGIOS_SERVICE_CHECK_PERFDATA_H
#define NAGIOS_SERVICE_CHECK_PERFDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosServiceCheckPerfData : public NagiosObject {
      public:
        NagiosServiceCheckPerfData(
            const nebstruct_service_check_data *serviceCheckData);
    };
} // namespace statusengine

#endif // !NAGIOS_SERVICE_CHECK_PERFDATA_H
