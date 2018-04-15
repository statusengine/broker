#ifndef NOTIFICATIONDATA_CALLBACK_H
#define NOTIFICATIONDATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class NotificationDataCallback
        : public NebmoduleCallback<nebstruct_notification_data> {
      public:
        NotificationDataCallback(Statusengine *se);

        virtual void Callback(int event_type,
                              nebstruct_notification_data *data);
    };
} // namespace statusengine

#endif // !NOTIFICATIONDATA_CALLBACK_H
