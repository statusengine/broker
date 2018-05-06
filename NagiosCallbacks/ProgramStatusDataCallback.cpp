#include "ProgramStatusDataCallback.h"

#include "NagiosObjects/NagiosProgramStatusData.h"
#include "Statusengine.h"

namespace statusengine {
    ProgramStatusDataCallback::ProgramStatusDataCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_PROGRAM_STATUS_DATA, se) {}

    void
    ProgramStatusDataCallback::Callback(int event_type,
                                        nebstruct_program_status_data *data) {
        NagiosProgramStatusData myData(data);
        se->SendMessage("statusngin_programmstatus", myData.ToString());
    }
} // namespace statusengine
