#include "NagiosNotificationData.h"

namespace statusengine {

    NagiosNotificationData::NagiosNotificationData(const nebstruct_notification_data *notificationData) {
        SetData<>("type", notificationData->type);
        SetData<>("flags", notificationData->flags);
        SetData<>("attr", notificationData->attr);
        SetData<>("timestamp", notificationData->timestamp.tv_sec);

        NagiosObject notification_data;

        notification_data.SetData<>("host_name", notificationData->host_name);
        notification_data.SetData<>("service_description", notificationData->service_description);
        notification_data.SetData<>("output", EncodeString(notificationData->output));
        notification_data.SetData<>("long_output", EncodeString(notificationData->output));
        notification_data.SetData<>("ack_author", notificationData->ack_author);
        notification_data.SetData<>("ack_data", notificationData->ack_data);
        notification_data.SetData<>("notification_type", notificationData->notification_type);
        notification_data.SetData<>("start_time", notificationData->start_time.tv_sec);
        notification_data.SetData<>("end_time", notificationData->end_time.tv_sec);
        notification_data.SetData<>("reason_type", notificationData->reason_type);
        notification_data.SetData<>("state", notificationData->state);
        notification_data.SetData<>("escalated", notificationData->escalated);
        notification_data.SetData<>("contacts_notified", notificationData->contacts_notified);

        SetData<>("notification_data", &notification_data);
    }
} // namespace statusengine
