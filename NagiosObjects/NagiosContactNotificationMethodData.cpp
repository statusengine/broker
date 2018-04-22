#include "NagiosContactNotificationMethodData.h"

namespace statusengine {

    NagiosContactNotificationMethodData::NagiosContactNotificationMethodData(
        const nebstruct_contact_notification_method_data
            *contactNotificationMethodData) {
        SetData<>("type", contactNotificationMethodData->type);
        SetData<>("flags", contactNotificationMethodData->flags);
        SetData<>("attr", contactNotificationMethodData->attr);
        SetData<>("timestamp", contactNotificationMethodData->timestamp.tv_sec);

        json contactnotificationmethod;
        SetData<>("host_name", contactNotificationMethodData->host_name,
                  &contactnotificationmethod);
        SetData<>("service_description",
                  contactNotificationMethodData->service_description,
                  &contactnotificationmethod);
        SetData<>("output", EncodeString(contactNotificationMethodData->output),
                  &contactnotificationmethod);
        SetData<>("ack_author", contactNotificationMethodData->ack_author,
                  &contactnotificationmethod);
        SetData<>("ack_data", contactNotificationMethodData->ack_data,
                  &contactnotificationmethod);
        SetData<>("contact_name", contactNotificationMethodData->contact_name,
                  &contactnotificationmethod);
        SetData<>("command_name", contactNotificationMethodData->command_name,
                  &contactnotificationmethod);
        SetData<>("command_args", contactNotificationMethodData->command_args,
                  &contactnotificationmethod);
        SetData<>("reason_type", contactNotificationMethodData->reason_type,
                  &contactnotificationmethod);
        SetData<>("state", contactNotificationMethodData->state,
                  &contactnotificationmethod);
        SetData<>("start_time",
                  contactNotificationMethodData->start_time.tv_sec,
                  &contactnotificationmethod);
        SetData<>("end_time", contactNotificationMethodData->end_time.tv_sec,
                  &contactnotificationmethod);

        SetData<>("contactnotificationmethod", contactnotificationmethod);
    }
} // namespace statusengine
