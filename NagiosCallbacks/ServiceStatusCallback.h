#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ServiceStatusCallback : public NebmoduleCallback<nebstruct_service_status_data> {
      public:
        explicit ServiceStatusCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_service_status_data *data);
    };
} // namespace statusengine
