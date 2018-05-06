#include "ServiceStatusCallback.h"

#include "NagiosObjects/NagiosServiceStatusData.h"
#include "Statusengine.h"

namespace statusengine {
    ServiceStatusCallback::ServiceStatusCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_SERVICE_STATUS_DATA, se) {}

    void ServiceStatusCallback::Callback(int event_type,
                                         nebstruct_service_status_data *data) {
        auto statusData = NagiosServiceStatusData(data);
        se->SendMessage("statusngin_servicestatus",
                        statusData.ToString());
    }
} // namespace statusengine
