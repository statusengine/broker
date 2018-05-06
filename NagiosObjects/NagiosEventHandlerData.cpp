#include "NagiosEventHandlerData.h"

namespace statusengine {

    NagiosEventHandlerData::NagiosEventHandlerData(
        const nebstruct_event_handler_data *eventHandlerData) {
        SetData<>("type", eventHandlerData->type);
        SetData<>("flags", eventHandlerData->flags);
        SetData<>("attr", eventHandlerData->attr);
        SetData<>("timestamp", eventHandlerData->timestamp.tv_sec);

        json_object *eventhandler = json_object_new_object();
        SetData<>("host_name", eventHandlerData->host_name, eventhandler);
        SetData<>("service_description", eventHandlerData->service_description,
                  eventhandler);
        SetData<>("output", EncodeString(eventHandlerData->output),
                  eventhandler);
        SetData<>("long_output", EncodeString(eventHandlerData->output),
                  eventhandler);
        SetData<>("command_name", eventHandlerData->command_name, eventhandler);
        SetData<>("command_args", eventHandlerData->command_args, eventhandler);
        SetData<>("command_line", eventHandlerData->command_line, eventhandler);
        SetData<>("state_type", eventHandlerData->state_type, eventhandler);
        SetData<>("state", eventHandlerData->state, eventhandler);
        SetData<>("timeout", eventHandlerData->timeout, eventhandler);
        SetData<>("early_timeout", eventHandlerData->early_timeout,
                  eventhandler);
        SetData<>("return_code", eventHandlerData->return_code, eventhandler);
        SetData<>("execution_time", eventHandlerData->execution_time,
                  eventhandler);
        SetData<>("start_time", eventHandlerData->start_time.tv_sec,
                  eventhandler);
        SetData<>("end_time", eventHandlerData->end_time.tv_sec, eventhandler);

        SetData<>("eventhandler", eventhandler);
    }
} // namespace statusengine
