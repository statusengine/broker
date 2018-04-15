#ifndef NAGIOS_DOWNTIMEDATA_H
#define NAGIOS_DOWNTIMEDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosDowntimeData : public NagiosObject {
      public:
        explicit NagiosDowntimeData(
            const nebstruct_downtime_data *downtimeData);
    };
} // namespace statusengine

#endif // !NAGIOS_DOWNTIMEDATA_H
