#include "HostCheckCallback.h"

#include "MessageHandler/MessageHandlerList.h"
#include "NagiosObjects/NagiosHostCheckData.h"
#include "Statusengine.h"

namespace statusengine {
    HostCheckCallback::HostCheckCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_HOST_CHECK_DATA, se), hostchecks(false), ochp(false), ochpBulk(false) {
        auto mHandler = se->GetMessageHandler();
        if (mHandler->QueueExists(Queue::HostCheck)) {
            hostCheckHandler = mHandler->GetMessageQueueHandler(Queue::HostCheck);
            hostchecks = true;
        }
        if (mHandler->QueueExists(Queue::OCHP)) {
            ochpHandler = mHandler->GetMessageQueueHandler(Queue::OCHP);
            ochp = true;
        }
        if (mHandler->QueueExists(Queue::BulkOCHP)) {
            bulkOCHPHandler = mHandler->GetMessageQueueHandler(Queue::BulkOCHP);
            ochpBulk = true;
        }
    }

    void HostCheckCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_host_check_data *>(vdata);

        if (data->type == NEBTYPE_HOSTCHECK_PROCESSED) {
            auto checkData = NagiosHostCheckData(data);
            auto msg = checkData.ToString();
            if (hostchecks) {
                hostCheckHandler->SendMessage(msg);
            }
            if (ochp) {
                ochpHandler->SendMessage(msg);
            }
            if (ochpBulk) {
                bulkOCHPHandler->SendBulkMessage(msg);
            }
        }
    }
} // namespace statusengine
