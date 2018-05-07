#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class NotificationDataCallback : public NebmoduleCallback<nebstruct_notification_data> {
      public:
        explicit NotificationDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_notification_data *data);
    };
} // namespace statusengine
