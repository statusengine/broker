#include "BulkMessageCallback.h"


namespace statusengine {
    BulkMessageCallback::BulkMessageCallback(IStatusengine *se, double interval)
        : EventCallback(se), interval(interval) {}

    double BulkMessageCallback::Interval() {
        return interval;
    }
    void BulkMessageCallback::Callback() {
        se->FlushBulkQueue();
    }
} // namespace statusengine
