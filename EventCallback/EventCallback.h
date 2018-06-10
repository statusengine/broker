#pragma once

namespace statusengine {
    class Statusengine;

    class EventCallback {
      public:
        EventCallback(Statusengine *se);

        virtual double Interval() = 0;
        virtual void Callback() = 0;

      protected:
        Statusengine *se;
        double interval;
    };
} // namespace statusengine
