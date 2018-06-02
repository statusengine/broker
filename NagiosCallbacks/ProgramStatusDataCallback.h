#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ProgramStatusDataCallback : public NebmoduleCallback {
      public:
        explicit ProgramStatusDataCallback(Statusengine *se);

        virtual void Callback(int event_type, void *vdata);
    };
} // namespace statusengine
