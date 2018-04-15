#ifndef NAGIOS_LOG_DATA_H
#define NAGIOS_LOG_DATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosLogData : public NagiosObject {
      public:
        explicit NagiosLogData(const nebstruct_log_data *logData);
    };
} // namespace statusengine

#endif // !NAGIOS_LOG_DATA_H
