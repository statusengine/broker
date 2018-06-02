#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class AcknowledgementDataCallback : public NebmoduleCallback {
      public:
        explicit AcknowledgementDataCallback(Statusengine *se);

        virtual void Callback(int event_type, void *data);
    };
} // namespace statusengine
