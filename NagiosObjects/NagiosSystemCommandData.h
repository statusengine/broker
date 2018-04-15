#ifndef NAGIOS_SYSTEM_COMMAND_DATA_H
#define NAGIOS_SYSTEM_COMMAND_DATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosSystemCommandData : public NagiosObject {
      public:
        NagiosSystemCommandData(
            const nebstruct_system_command_data *systemCommandData);
    };
} // namespace statusengine

#endif // !NAGIOS_SYSTEM_COMMAND_DATA_H
