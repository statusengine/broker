#ifndef NAGIOS_HOST_CHECK_DATA_H
#define NAGIOS_HOST_CHECK_DATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosHostCheckData : public NagiosObject {
      public:
        explicit NagiosHostCheckData(
            const nebstruct_host_check_data *hostCheckData);
    };
} // namespace statusengine

#endif // !NAGIOS_HOST_CHECK_DATA_H
