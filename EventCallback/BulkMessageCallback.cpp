#include "BulkMessageCallback.h"

#include "Statusengine.h"

namespace statusengine {
    BulkMessageCallback::BulkMessageCallback(Statusengine *se, double interval)
        : EventCallback(se), interval(interval) {}

    double BulkMessageCallback::Interval() {
        return interval;
    }
    void BulkMessageCallback::Callback() {
        se->FlushBulkQueue();
    }
} // namespace statusengine
