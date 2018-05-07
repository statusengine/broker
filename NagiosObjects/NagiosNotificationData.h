#pragma once

#include "NagiosObject.h"
#include "nebmodule.h"

namespace statusengine {
    class NagiosNotificationData : public NagiosObject {
      public:
        explicit NagiosNotificationData(const nebstruct_notification_data *notificationData);
    };
} // namespace statusengine
