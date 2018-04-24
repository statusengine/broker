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
            json restartData = {{"object_type", NEBTYPE_PROCESS_RESTART}};
            se->SendMessage("statusngin_core_restart", restartData.dump());
        }

        if (process_data) {
            NagiosProcessData processData(data);
            se->SendMessage("statusngin_processdata",
                            processData.GetData().dump());
        }
    }
} // namespace statusengine
