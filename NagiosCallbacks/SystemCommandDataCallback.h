#ifndef SYSTEM_COMMAND_DATA_CALLBACK_H
#define SYSTEM_COMMAND_DATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class SystemCommandDataCallback
        : public NebmoduleCallback<nebstruct_system_command_data> {
      public:
        SystemCommandDataCallback(Statusengine *se);

        virtual void Callback(int event_type,
                              nebstruct_system_command_data *data);
    };
} // namespace statusengine

#endif // !SYSTEM_COMMAND_DATA_CALLBACK_H
