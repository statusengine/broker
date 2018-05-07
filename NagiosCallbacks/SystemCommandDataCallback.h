#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class SystemCommandDataCallback : public NebmoduleCallback<nebstruct_system_command_data> {
      public:
        explicit SystemCommandDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_system_command_data *data);
    };
} // namespace statusengine
