#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ServiceCheckCallback : public NebmoduleCallback {
      public:
        explicit ServiceCheckCallback(Statusengine *se, bool servicechecks, bool ocsp, bool ocspBulk,
                                      bool service_perfdata);

        virtual void Callback(int event_type, void *vdata);

      private:
        bool servicechecks;
        bool ocsp;
        bool ocspBulk;
        bool service_perfdata;
    };
} // namespace statusengine
