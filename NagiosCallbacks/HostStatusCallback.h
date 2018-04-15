#ifndef HOSTSTATUS_CALLBACK_H
#define HOSTSTATUS_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class HostStatusCallback
        : public NebmoduleCallback<nebstruct_host_status_data> {
      public:
        HostStatusCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_host_status_data *data);
    };
} // namespace statusengine

#endif // !HOSTSTATUS_CALLBACK_H
