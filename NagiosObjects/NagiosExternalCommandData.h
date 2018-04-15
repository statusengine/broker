#ifndef NAGIOS_EXTERNALCOMMANDDATA_H
#define NAGIOS_EXTERNALCOMMANDDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosExternalCommandData : public NagiosObject {
      public:
        NagiosExternalCommandData(
            const nebstruct_external_command_data *externalCommandData);
    };
} // namespace statusengine

#endif // !NAGIOS_EXTERNALCOMMANDDATA_H
