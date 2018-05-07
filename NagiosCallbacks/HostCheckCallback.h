#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class HostCheckCallback : public NebmoduleCallback<nebstruct_host_check_data> {
      public:
        explicit HostCheckCallback(Statusengine *se, bool hostchecks, bool ocsp);

        virtual void Callback(int event_type, nebstruct_host_check_data *data);

      private:
        bool hostchecks;
        bool ocsp;
    };
} // namespace statusengine
