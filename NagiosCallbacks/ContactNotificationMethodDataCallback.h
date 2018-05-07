#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ContactNotificationMethodDataCallback : public NebmoduleCallback<nebstruct_contact_notification_method_data> {
      public:
        explicit ContactNotificationMethodDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_contact_notification_method_data *data);
    };
} // namespace statusengine
