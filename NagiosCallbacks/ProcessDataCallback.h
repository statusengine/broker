#pragma once

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ProcessDataCallback : public NebmoduleCallback<nebstruct_process_data> {
      public:
        explicit ProcessDataCallback(Statusengine *se, bool restart_data, bool process_data);

        virtual void Callback(int event_type, nebstruct_process_data *data);

      private:
        bool restart_data;
        bool process_data;
    };
} // namespace statusengine
