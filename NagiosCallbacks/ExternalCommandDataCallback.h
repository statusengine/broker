#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ExternalCommandDataCallback : public NebmoduleCallback {
      public:
        explicit ExternalCommandDataCallback(Statusengine *se);

        virtual void Callback(int event_type, void *vdata);
    };
} // namespace statusengine
