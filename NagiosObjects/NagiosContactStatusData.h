#ifndef NAGIOS_CONTACTSTATUSDATA_H
#define NAGIOS_CONTACTSTATUSDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosContactStatusData : public NagiosObject {
      public:
        explicit NagiosContactStatusData(const nebstruct_contact_status_data *contactStatusData);
    };
} // namespace statusengine

#endif // !NAGIOS_CONTACTSTATUSDATA_H
