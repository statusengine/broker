#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class StateChangeCallback : public NebmoduleCallback {
      public:
        explicit StateChangeCallback(Statusengine *se);

        virtual void Callback(int event_type, void *vdata);
    };
} // namespace statusengine
