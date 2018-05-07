#include "NagiosContactNotificationData.h"

namespace statusengine {

    NagiosContactNotificationData::NagiosContactNotificationData(
        const nebstruct_contact_notification_data *contactNotificationData) {
        SetData<>("type", contactNotificationData->type);
        SetData<>("flags", contactNotificationData->flags);
        SetData<>("attr", contactNotificationData->attr);
        SetData<>("timestamp", contactNotificationData->timestamp.tv_sec);

        NagiosObject contactnotificationdata;
        contactnotificationdata.SetData<>("host_name", contactNotificationData->host_name);
        contactnotificationdata.SetData<>("service_description", contactNotificationData->service_description);
        contactnotificationdata.SetData<>("output", EncodeString(contactNotificationData->output));
        contactnotificationdata.SetData<>("long_output", EncodeString(contactNotificationData->output));
        contactnotificationdata.SetData<>("ack_author", contactNotificationData->ack_author);
        contactnotificationdata.SetData<>("ack_data", contactNotificationData->ack_data);
        contactnotificationdata.SetData<>("contact_name", contactNotificationData->contact_name);
        contactnotificationdata.SetData<>("state", contactNotificationData->state);
        contactnotificationdata.SetData<>("reason_type", contactNotificationData->reason_type);
        contactnotificationdata.SetData<>("end_time", contactNotificationData->end_time.tv_sec);
        contactnotificationdata.SetData<>("start_time", contactNotificationData->start_time.tv_sec);
        contactnotificationdata.SetData<>("notification_type", contactNotificationData->notification_type);

        SetData<>("contactnotificationdata", &contactnotificationdata);
    }
} // namespace statusengine
