#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class StateChangeCallback : public NebmoduleCallback<nebstruct_statechange_data> {
      public:
        explicit StateChangeCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_statechange_data *data);
    };
} // namespace statusengine
