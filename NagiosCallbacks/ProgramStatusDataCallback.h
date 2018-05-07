#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ProgramStatusDataCallback : public NebmoduleCallback<nebstruct_program_status_data> {
      public:
        explicit ProgramStatusDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_program_status_data *data);
    };
} // namespace statusengine
