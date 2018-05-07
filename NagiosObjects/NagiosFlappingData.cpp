#include "NagiosFlappingData.h"

namespace statusengine {

    NagiosFlappingData::NagiosFlappingData(const nebstruct_flapping_data *flappingData) {
        SetData<>("type", flappingData->type);
        SetData<>("flags", flappingData->flags);
        SetData<>("attr", flappingData->attr);
        SetData<>("timestamp", flappingData->timestamp.tv_sec);

        comment *tmpComment = nullptr;
        if (flappingData->flapping_type == SERVICE_FLAPPING) {
            tmpComment = find_service_comment(flappingData->comment_id);
        }
        else {
            tmpComment = find_host_comment(flappingData->comment_id);
        }

        json_object *flapping = json_object_new_object();
        SetData<>("host_name", flappingData->host_name, flapping);
        SetData<>("service_description", flappingData->service_description, flapping);
        SetData<>("flapping_type", flappingData->flapping_type, flapping);
        SetData<>("comment_id", flappingData->comment_id, flapping);
        SetData<>("comment_entry_time", (tmpComment == nullptr ? 0 : tmpComment->entry_time), flapping);
        SetData<>("percent_change", flappingData->percent_change, flapping);
        SetData<>("high_threshold", flappingData->high_threshold, flapping);
        SetData<>("low_threshold", flappingData->low_threshold, flapping);

        SetData<>("flapping", flapping);
    }
} // namespace statusengine
