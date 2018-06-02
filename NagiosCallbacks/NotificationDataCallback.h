#pragma once

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class NotificationDataCallback : public NebmoduleCallback {
      public:
        explicit NotificationDataCallback(Statusengine *se);

        virtual void Callback(int event_type, void *vdata);
    };
} // namespace statusengine
