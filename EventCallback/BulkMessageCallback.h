#pragma once

#include "EventCallback.h"

namespace statusengine {
    class BulkMessageCallback : public EventCallback {
      public:
        BulkMessageCallback(Statusengine *se);

        virtual double Interval();
        virtual void Callback();
    };
} // namespace statusengine
