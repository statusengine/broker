#include "NagiosDowntimeData.h"

namespace statusengine {

    NagiosDowntimeData::NagiosDowntimeData(const nebstruct_downtime_data *downtimeData) {
        SetData<>("type", downtimeData->type);
        SetData<>("flags", downtimeData->flags);
        SetData<>("attr", downtimeData->attr);
        SetData<>("timestamp", downtimeData->timestamp.tv_sec);

        json_object *downtime = json_object_new_object();
        SetData<>("host_name", downtimeData->host_name, downtime);
        SetData<>("service_description", downtimeData->service_description, downtime);
        SetData<>("author_name", downtimeData->author_name, downtime);
        SetData<>("comment_data", downtimeData->comment_data, downtime);
        SetData<>("host_name", downtimeData->host_name, downtime);
        SetData<>("downtime_type", downtimeData->downtime_type, downtime);
        SetData<>("entry_time", downtimeData->entry_time, downtime);
        SetData<>("start_time", downtimeData->start_time, downtime);
        SetData<>("end_time", downtimeData->end_time, downtime);
        SetData<>("triggered_by", downtimeData->triggered_by, downtime);
        SetData<>("downtime_id", downtimeData->downtime_id, downtime);
        SetData<>("fixed", downtimeData->fixed, downtime);
        SetData<>("duration", downtimeData->duration, downtime);

        SetData<>("downtime", downtime);
    }
} // namespace statusengine
