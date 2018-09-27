#include "ServiceCheckCallback.h"

#include "MessageHandler/MessageHandlerList.h"
#include "NagiosObjects/NagiosServiceCheckData.h"
#include "NagiosObjects/NagiosServiceCheckPerfData.h"
#include "Statusengine.h"

namespace statusengine {
    ServiceCheckCallback::ServiceCheckCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_SERVICE_CHECK_DATA, se), servicechecks(false), ocsp(false),
          service_perfdata(false) {
        auto mHandler = se->GetMessageHandler();
        if (mHandler->QueueExists(Queue::ServiceCheck)) {
            serviceCheckHandler = mHandler->GetMessageQueueHandler(Queue::ServiceCheck);
            servicechecks = true;
        }
        if (mHandler->QueueExists(Queue::OCSP)) {
            ocspHandler = mHandler->GetMessageQueueHandler(Queue::OCSP);
            ocsp = true;
        }
        if (mHandler->QueueExists(Queue::ServicePerfData)) {
            servicePerfHandler = mHandler->GetMessageQueueHandler(Queue::ServicePerfData);
            service_perfdata = true;
        }
    }

    void ServiceCheckCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_service_check_data *>(vdata);

        if (data->type == NEBTYPE_SERVICECHECK_PROCESSED) {
            if (servicechecks || ocsp) {
                auto checkData = new NagiosServiceCheckData(data);
                if (servicechecks) {
                    serviceCheckHandler->SendMessage(checkData);
                }
                if (ocsp) {
                    ocspHandler->SendMessage(checkData);
                }
            }
            if (service_perfdata) {
                auto checkPerfData = new NagiosServiceCheckPerfData(data);
                servicePerfHandler->SendMessage(checkPerfData);
            }
        }
    }
} // namespace statusengine
