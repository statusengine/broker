#ifndef NAGIOS_CONTACTNOTIFICATIONMETHODDATA_H
#define NAGIOS_CONTACTNOTIFICATIONMETHODDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosContactNotificationMethodData : public NagiosObject {
      public:
        NagiosContactNotificationMethodData(
            const nebstruct_contact_notification_method_data
                *contactNotificationMethodData);
    };
} // namespace statusengine

#endif // !NAGIOS_CONTACTNOTIFICATIONMETHODDATA_H
