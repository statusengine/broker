#pragma once

#include "IStatusengine.h"


namespace statusengine {

    class EventCallback {
      public:
        explicit EventCallback(IStatusengine *se) : se(se) {}
        virtual ~EventCallback() = default;

        virtual double Interval() = 0;
        virtual void Callback() = 0;

      protected:
        IStatusengine *se;
    };

    class MessageWorkerCallback : public EventCallback {
    public:
        MessageWorkerCallback(IStatusengine *se, double interval)
                : EventCallback(se), interval(interval) {}

        double Interval() override {
            return interval;
        }
        void Callback() override {
            se->GetMessageHandler()->Worker();
        }

    private:
        double interval;
    };

    class BulkMessageCallback : public EventCallback {
    public:
        explicit BulkMessageCallback(IStatusengine *se, double interval)
                : EventCallback(se), interval(interval) {}

        double Interval() override {
            return interval;
        }
        void Callback() override {
            se->FlushBulkQueue();
        }

    private:
        double interval;
    };
} // namespace statusengine
