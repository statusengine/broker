#include "HostCheckCallback.h"

#include "NagiosObjects/NagiosHostCheckData.h"
#include "Statusengine.h"

namespace statusengine {
    HostCheckCallback::HostCheckCallback(Statusengine *se, bool hostchecks, bool ocsp)
        : NebmoduleCallback(NEBCALLBACK_HOST_CHECK_DATA, se), hostchecks(hostchecks), ocsp(ocsp) {}

    void HostCheckCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_host_check_data *>(vdata);

        if (data->type == NEBTYPE_HOSTCHECK_PROCESSED) {
            auto checkData = NagiosHostCheckData(data);
            if (hostchecks) {
                se->SendMessage("statusngin_hostchecks", checkData.ToString());
            }
            if (ocsp) {
                se->SendMessage("statusngin_ochp", checkData.ToString());
            }
        }
    }
} // namespace statusengine
