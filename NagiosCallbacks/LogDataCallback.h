#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class LogDataCallback : public NebmoduleCallback {
      public:
        explicit LogDataCallback(Statusengine *se);

        virtual void Callback(int event_type, void *vdata);
    };
} // namespace statusengine
