#ifndef NAGIOS_CONTACTNOTIFICATIONDATA_H
#define NAGIOS_CONTACTNOTIFICATIONDATA_H

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosContactNotificationData : public NagiosObject {
      public:
        explicit NagiosContactNotificationData(
            const nebstruct_contact_notification_data *contactNotificationData);
    };
} // namespace statusengine

#endif // !NAGIOS_CONTACTNOTIFICATIONDATA_H
