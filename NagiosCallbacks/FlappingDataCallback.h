#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class FlappingDataCallback : public NebmoduleCallback<nebstruct_flapping_data> {
      public:
        explicit FlappingDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_flapping_data *data);
    };
} // namespace statusengine
