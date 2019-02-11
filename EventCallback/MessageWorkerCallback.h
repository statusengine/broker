#pragma once

#include "EventCallback.h"

namespace statusengine {
    class MessageWorkerCallback : public EventCallback {
      public:
        MessageWorkerCallback(IStatusengine *se, double interval);

        virtual double Interval();
        virtual void Callback();

      private:
        double interval;
    };
} // namespace statusengine
