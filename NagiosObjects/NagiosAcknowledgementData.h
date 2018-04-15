#ifndef NAGIOS_ACKNOWLEDGEMENTDATA_H
#define NAGIOS_ACKNOWLEDGEMENTDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosAcknowledgementData : public NagiosObject {
      public:
        explicit NagiosAcknowledgementData(
            const nebstruct_acknowledgement_data *acknowledgementData);
    };
} // namespace statusengine

#endif // !NAGIOS_ACKNOWLEDGEMENTDATA_H
