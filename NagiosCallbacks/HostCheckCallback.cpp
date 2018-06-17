#include "HostCheckCallback.h"

#include "NagiosObjects/NagiosHostCheckData.h"
#include "Statusengine.h"

namespace statusengine {
    HostCheckCallback::HostCheckCallback(Statusengine *se, bool hostchecks, bool ocsp, bool ochpBulk)
        : NebmoduleCallback(NEBCALLBACK_HOST_CHECK_DATA, se), hostchecks(hostchecks), ochp(ochp), ochpBulk(ochpBulk) {}

    void HostCheckCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_host_check_data *>(vdata);

        if (data->type == NEBTYPE_HOSTCHECK_PROCESSED) {
            auto checkData = NagiosHostCheckData(data);
            auto msg = checkData.ToString();
            if (hostchecks) {
                se->SendMessage("statusngin_hostchecks", msg);
            }
            if (ochp) {
                se->SendMessage("statusngin_ochp", msg);
            }
            if (ochpBulk) {
                se->SendBulkMessage("BulkOCHP", msg);
            }
        }
    }
} // namespace statusengine
