#include "LogDataCallback.h"

#include "NagiosObjects/NagiosLogData.h"
#include "Statusengine.h"

namespace statusengine {
    LogDataCallback::LogDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_LOG_DATA, se) {}

    void LogDataCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_log_data *>(vdata);

        auto logData = NagiosLogData(data);
        se->SendMessage("statusngin_logentries", logData.ToString());
    }
} // namespace statusengine
