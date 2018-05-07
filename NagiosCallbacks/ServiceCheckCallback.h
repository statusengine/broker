#ifndef SERVICECHECK_CALLBACK_H
#define SERVICECHECK_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ServiceCheckCallback : public NebmoduleCallback<nebstruct_service_check_data> {
      public:
        explicit ServiceCheckCallback(Statusengine *se, bool servicechecks, bool ocsp, bool service_perfdata);

        virtual void Callback(int event_type, nebstruct_service_check_data *data);

      private:
        bool servicechecks;
        bool ocsp;
        bool service_perfdata;
    };
} // namespace statusengine

#endif // !SERVICECHECK_CALLBACK_H
