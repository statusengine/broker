#pragma once

#include "IStatusengine.h"


namespace statusengine {

    class EventCallback {
      public:
        explicit EventCallback(IStatusengine &se) : se(se) {}
        virtual ~EventCallback() = default;

        virtual double Interval() = 0;

        /**
         * Runs the callback.
         *
         * @return true if it wants the next run as soon as possible rather than after
         *         Interval() seconds, because it stopped with work left over.
         */
        virtual bool Callback() = 0;

      protected:
        IStatusengine &se;
    };

    class MessageWorkerCallback : public EventCallback {
    public:
        MessageWorkerCallback(IStatusengine &se, double interval)
                : EventCallback(se), interval(interval) {}

        double Interval() override {
            return interval;
        }
        bool Callback() override {
            return se.GetMessageHandler()->Worker();
        }

    private:
        double interval;
    };

    class BulkMessageCallback : public EventCallback {
    public:
        explicit BulkMessageCallback(IStatusengine &se, double interval)
                : EventCallback(se), interval(interval) {}

        double Interval() override {
            return interval;
        }
        bool Callback() override {
            se.FlushBulkQueue();
            return false;
        }

    private:
        double interval;
    };
} // namespace statusengine
