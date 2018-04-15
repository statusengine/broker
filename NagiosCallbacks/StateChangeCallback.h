#ifndef STATECHANGE_CALLBACK_H
#define STATECHANGE_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class StateChangeCallback
        : public NebmoduleCallback<nebstruct_statechange_data> {
      public:
        StateChangeCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_statechange_data *data);
    };
} // namespace statusengine

#endif // !STATECHANGE_CALLBACK_H
