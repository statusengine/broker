#pragma once

#include "EventCallback.h"

namespace statusengine {
    class BulkMessageCallback : public EventCallback {
      public:
        BulkMessageCallback(Statusengine *se, double interval);

        virtual double Interval();
        virtual void Callback();

      private:
        double interval;
    };
} // namespace statusengine
