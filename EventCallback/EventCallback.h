#pragma once

#include "IStatusengine.h"


namespace statusengine {

    class EventCallback {
      public:
        explicit EventCallback(IStatusengine *se);
        virtual ~EventCallback() = default;

        virtual double Interval() = 0;
        virtual void Callback() = 0;

      protected:
        IStatusengine *se;
        double interval;
    };
} // namespace statusengine
