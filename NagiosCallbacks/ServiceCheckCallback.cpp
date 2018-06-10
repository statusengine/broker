#include "ServiceCheckCallback.h"

#include "NagiosObjects/NagiosServiceCheckData.h"
#include "NagiosObjects/NagiosServiceCheckPerfData.h"
#include "Statusengine.h"

namespace statusengine {
    ServiceCheckCallback::ServiceCheckCallback(Statusengine *se, bool servicechecks, bool ocsp, bool ocspBulk,
                                               bool service_perfdata)
        : NebmoduleCallback(NEBCALLBACK_SERVICE_CHECK_DATA, se), servicechecks(servicechecks), ocsp(ocsp),
          ocspBulk(ocspBulk), service_perfdata(service_perfdata) {}

    void ServiceCheckCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_service_check_data *>(vdata);

        if (data->type == NEBTYPE_SERVICECHECK_PROCESSED) {
            if (servicechecks || ocsp) {
                auto checkData = NagiosServiceCheckData(data);
                auto msg = checkData.ToString();
                if (servicechecks) {
                    se->SendMessage("statusngin_servicechecks", msg);
                }
                if (ocsp) {
                    se->SendMessage("statusngin_ocsp", msg);
                }
                if (ocspBulk) {
                    se->SendBulkMessage("BulkOCSP", msg);
                }
            }
            if (service_perfdata) {
                auto checkPerfData = NagiosServiceCheckPerfData(data);
                se->SendMessage("statusngin_service_perfdata", checkPerfData.ToString());
            }
        }
    }
} // namespace statusengine
