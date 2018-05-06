#include "ProcessDataCallback.h"

#include "LogStream.h"
#include "NagiosObjects/NagiosProcessData.h"
#include "Statusengine.h"

namespace statusengine {
    ProcessDataCallback::ProcessDataCallback(Statusengine *se,
                                             bool restart_data,
                                             bool process_data)
        : NebmoduleCallback(NEBCALLBACK_PROCESS_DATA, se),
          restart_data(restart_data), process_data(process_data) {}

    void ProcessDataCallback::Callback(int event_type,
                                       nebstruct_process_data *data) {
        if (restart_data && data->type == NEBTYPE_PROCESS_START) {
            json_object *restartData = json_object_new_object();
            json_object_object_add(
                restartData, "object_type",
                json_object_new_int(NEBTYPE_PROCESS_RESTART));
            se->SendMessage(
                "statusngin_core_restart",
                std::string(json_object_to_json_string(restartData)));
            json_object_put(restartData);
        }

        if (process_data) {
            NagiosProcessData processData(data);
            se->SendMessage("statusngin_processdata", processData.ToString());
        }
    }
} // namespace statusengine
