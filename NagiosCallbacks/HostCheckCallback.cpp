#include "HostCheckCallback.h"

#include "MessageHandler/MessageHandlerList.h"
#include "NagiosObject.h"
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

    HostCheckCallback::HostCheckCallback(HostCheckCallback &&other) noexcept
        : NebmoduleCallback::NebmoduleCallback(std::move(other)), hostchecks(other.hostchecks), ochp(other.ochp),
          hostCheckHandler(std::move(other.hostCheckHandler)), ochpHandler(std::move(other.ochpHandler)) {}

    void HostCheckCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_host_check_data *>(vdata);

        if (data->type == NEBTYPE_HOSTCHECK_PROCESSED) {
            NagiosHostCheckData checkData(data);
            if (hostchecks) {
                hostCheckHandler->SendMessage(checkData);
            }
            if (ochp) {
                ochpHandler->SendMessage(checkData);
            }
        }
    }
} // namespace statusengine
