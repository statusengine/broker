#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class LogDataCallback : public NebmoduleCallback<nebstruct_log_data> {
      public:
        explicit LogDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_log_data *data);
    };
} // namespace statusengine
