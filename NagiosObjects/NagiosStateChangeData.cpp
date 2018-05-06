#include "NagiosStateChangeData.h"

#include "NagiosService.h"

namespace statusengine {

    NagiosStateChangeData::NagiosStateChangeData(
        const nebstruct_statechange_data *stateChangeData) {
        SetData<>("type", stateChangeData->type);
        SetData<>("flags", stateChangeData->flags);
        SetData<>("attr", stateChangeData->attr);
        SetData<>("timestamp", stateChangeData->timestamp.tv_sec);

        int last_state;
        int last_hard_state;

        if (stateChangeData->statechange_type == SERVICE_STATECHANGE) {
            auto tmp_service =
                reinterpret_cast<service *>(stateChangeData->object_ptr);
            last_state = tmp_service->last_state;
            last_hard_state = tmp_service->last_hard_state;
        }
        else {
            auto tmp_host =
                reinterpret_cast<host *>(stateChangeData->object_ptr);
            last_state = tmp_host->last_state;
            last_hard_state = tmp_host->last_hard_state;
        }

        json_object *statechange = json_object_new_object();

        SetData<>("host_name", stateChangeData->host_name, statechange);
        SetData<>("service_description", stateChangeData->service_description,
                  statechange);
        SetData<>("output", EncodeString(stateChangeData->output), statechange);
        SetData<>("long_output", EncodeString(stateChangeData->output),
                  statechange);
        SetData<>("statechange_type", stateChangeData->statechange_type,
                  statechange);
        SetData<>("state", stateChangeData->state, statechange);
        SetData<>("state_type", stateChangeData->state_type, statechange);
        SetData<>("current_attempt", stateChangeData->current_attempt,
                  statechange);
        SetData<>("max_attempts", stateChangeData->max_attempts, statechange);
        SetData<>("last_state", last_state, statechange);
        SetData<>("last_hard_state", last_hard_state, statechange);

        SetData<>("statechange", statechange);
    }
} // namespace statusengine
