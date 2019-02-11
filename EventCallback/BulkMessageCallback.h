#pragma once

#include "EventCallback.h"

namespace statusengine {
    class BulkMessageCallback : public EventCallback {
      public:
        explicit BulkMessageCallback(IStatusengine *se, double interval);

        double Interval() override;
        void Callback() override;

      private:
        double interval;
    };
} // namespace statusengine
