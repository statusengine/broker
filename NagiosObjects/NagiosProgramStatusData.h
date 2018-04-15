#ifndef NAGIOS_PROGRAMSTATUSDATA_H
#define NAGIOS_PROGRAMSTATUSDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosProgramStatusData : public NagiosObject {
      public:
        NagiosProgramStatusData(
            const nebstruct_program_status_data *programmStatusData);
    };
} // namespace statusengine

#endif // !NAGIOS_PROGRAMSTATUSDATA_H
