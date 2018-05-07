#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ContactNotificationDataCallback : public NebmoduleCallback<nebstruct_contact_notification_data> {
      public:
        explicit ContactNotificationDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_contact_notification_data *data);
    };
} // namespace statusengine
