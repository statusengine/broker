#pragma once

#include "NagiosObject.h"
#include "Nebmodule.h"

namespace statusengine {
    class NagiosContactNotificationData : public NagiosObject {
      public:
        explicit NagiosContactNotificationData(const nebstruct_contact_notification_data *contactNotificationData);
    };
} // namespace statusengine
