#include "HostStatusCallback.h"

#include "NagiosObjects/NagiosHostStatusData.h"
#include "Statusengine.h"

namespace statusengine {
    HostStatusCallback::HostStatusCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_HOST_STATUS_DATA, se) {}

    void HostStatusCallback::Callback(int event_type,
                                      nebstruct_host_status_data *data) {
        auto statusData = NagiosHostStatusData(data);
        se->SendMessage("statusngin_hoststatus", statusData.ToString());
    }
} // namespace statusengine
