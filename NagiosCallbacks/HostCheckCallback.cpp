#include "HostCheckCallback.h"

#include "MessageHandler/MessageHandlerList.h"
#include "NagiosObjects/NagiosHostCheckData.h"
#include "Statusengine.h"

namespace statusengine {
    HostCheckCallback::HostCheckCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_HOST_CHECK_DATA, se), hostchecks(false), ochp(false) {
        auto mHandler = se->GetMessageHandler();
        if (mHandler->QueueExists(Queue::HostCheck)) {
            hostCheckHandler = mHandler->GetMessageQueueHandler(Queue::HostCheck);
            hostchecks = true;
        }
        if (mHandler->QueueExists(Queue::OCHP)) {
            ochpHandler = mHandler->GetMessageQueueHandler(Queue::OCHP);
            ochp = true;
        }
    }

    void HostCheckCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_host_check_data *>(vdata);

        if (data->type == NEBTYPE_HOSTCHECK_PROCESSED) {
            auto checkData = new NagiosHostCheckData(data);
            if (hostchecks) {
                hostCheckHandler->SendMessage(checkData);
            }
            if (ochp) {
                ochpHandler->SendMessage(checkData);
            }
        }
    }
} // namespace statusengine
