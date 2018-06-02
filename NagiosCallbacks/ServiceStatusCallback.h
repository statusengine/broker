#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ServiceStatusCallback : public NebmoduleCallback {
      public:
        explicit ServiceStatusCallback(Statusengine *se);

        virtual void Callback(int event_type, void *data);
    };
} // namespace statusengine
