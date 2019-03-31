#include "ServiceCheckCallback.h"

#include "MessageHandler/MessageHandlerList.h"
#include "NagiosObject.h"
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

    ServiceCheckCallback::ServiceCheckCallback(ServiceCheckCallback &&other) noexcept
        : NebmoduleCallback::NebmoduleCallback(std::move(other)), servicechecks(other.servicechecks), ocsp(other.ocsp),
          service_perfdata(other.service_perfdata), serviceCheckHandler(std::move(other.serviceCheckHandler)),
          ocspHandler(std::move(other.ocspHandler)), servicePerfHandler(std::move(other.servicePerfHandler)) {}

    void ServiceCheckCallback::Callback(int event_type, void *vdata) {
        auto data = reinterpret_cast<nebstruct_service_check_data *>(vdata);
        auto temp_service = reinterpret_cast<service*>(data->object_ptr);

        if (data->type == NEBTYPE_SERVICECHECK_PROCESSED) {
            if (servicechecks || ocsp) {
                NagiosServiceCheckData checkData(data);
                ;
                if (servicechecks) {
                    serviceCheckHandler->SendMessage(checkData);
                }
                if (ocsp) {
                    ocspHandler->SendMessage(checkData);
                }
            }
            if (service_perfdata && temp_service->process_performance_data != 0) {
                NagiosServiceCheckPerfData checkPerfData(data);
                servicePerfHandler->SendMessage(checkPerfData);
            }
        }
    }
} // namespace statusengine
