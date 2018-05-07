#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class HostStatusCallback : public NebmoduleCallback<nebstruct_host_status_data> {
      public:
        explicit HostStatusCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_host_status_data *data);
    };
} // namespace statusengine
