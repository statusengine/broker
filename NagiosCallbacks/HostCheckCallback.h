#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class HostCheckCallback : public NebmoduleCallback {
      public:
        explicit HostCheckCallback(Statusengine *se, bool hostchecks, bool ocsp);

        virtual void Callback(int event_type, void *vdata);

      private:
        bool hostchecks;
        bool ocsp;
    };
} // namespace statusengine
