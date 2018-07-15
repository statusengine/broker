#pragma once

#include "EventCallback.h"

namespace statusengine {
    class MessageWorkerCallback : public EventCallback {
      public:
        MessageWorkerCallback(Statusengine *se, double interval);

        virtual double Interval();
        virtual void Callback();

      private:
        double interval;
    };
} // namespace statusengine
