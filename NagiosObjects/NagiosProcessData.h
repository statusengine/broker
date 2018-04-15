#ifndef NAGIOS_PROCESS_DATA_H
#define NAGIOS_PROCESS_DATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosProcessData : public NagiosObject {
      public:
        explicit NagiosProcessData(const nebstruct_process_data *processData);
    };
} // namespace statusengine

#endif // !NAGIOS_PROCESS_DATA_H
