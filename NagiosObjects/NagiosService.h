#ifndef NAGIOS_SERVICE_H
#define NAGIOS_SERVICE_H

#include "nebmodule.h"

#include "NagiosObject.h"

namespace statusengine {
    class NagiosService : public NagiosObject {
      public:
        explicit NagiosService(const service *data);
    };

} // namespace statusengine

#endif // !NAGIOS_SERVICE_H
