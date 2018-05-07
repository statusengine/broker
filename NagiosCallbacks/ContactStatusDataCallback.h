#ifndef CONTACTSTATUSDATA_CALLBACK_H
#define CONTACTSTATUSDATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ContactStatusDataCallback : public NebmoduleCallback<nebstruct_contact_status_data> {
      public:
        explicit ContactStatusDataCallback(Statusengine *se);

        virtual void Callback(int event_type, nebstruct_contact_status_data *data);
    };
} // namespace statusengine

#endif // !CONTACTSTATUSDATA_CALLBACK_H
