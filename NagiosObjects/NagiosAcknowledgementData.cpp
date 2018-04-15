#include "NagiosAcknowledgementData.h"

namespace statusengine {

    NagiosAcknowledgementData::NagiosAcknowledgementData(
        const nebstruct_acknowledgement_data *acknowledgementData) {
        SetData<>("type", acknowledgementData->type);
        SetData<>("flags", acknowledgementData->flags);
        SetData<>("attr", acknowledgementData->attr);
        SetData<>("timestamp", acknowledgementData->timestamp.tv_sec);

        json acknowledgement;
        SetData<>("host_name", acknowledgementData->host_name,
                  &acknowledgement);
        SetData<>("service_description",
                  acknowledgementData->service_description, &acknowledgement);
        SetData<>("author_name", acknowledgementData->author_name,
                  &acknowledgement);
        SetData<>("comment_data", acknowledgementData->comment_data,
                  &acknowledgement);
        SetData<>("acknowledgement_type",
                  acknowledgementData->acknowledgement_type, &acknowledgement);
        SetData<>("state", acknowledgementData->state, &acknowledgement);
        SetData<>("is_sticky", acknowledgementData->is_sticky,
                  &acknowledgement);
        SetData<>("persistent_comment", acknowledgementData->persistent_comment,
                  &acknowledgement);
        SetData<>("notify_contacts", acknowledgementData->notify_contacts,
                  &acknowledgement);

        SetData<>("acknowledgement", acknowledgement);
    }
} // namespace statusengine
