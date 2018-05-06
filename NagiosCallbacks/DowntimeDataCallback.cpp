#include "DowntimeDataCallback.h"

#include "NagiosObjects/NagiosDowntimeData.h"
#include "Statusengine.h"

namespace statusengine {
    DowntimeDataCallback::DowntimeDataCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_DOWNTIME_DATA, se) {}

    void DowntimeDataCallback::Callback(int event_type,
                                        nebstruct_downtime_data *data) {
        auto myData = NagiosDowntimeData(data);
        se->SendMessage("statusngin_downtimes", myData.ToString());
    }
} // namespace statusengine
