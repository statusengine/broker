#include "BulkMessageCallback.h"

#include "Statusengine.h"

namespace statusengine {
    BulkMessageCallback::BulkMessageCallback(Statusengine *se) : EventCallback(se) {}

    double BulkMessageCallback::Interval() {
        return 5.0;
    }
    void BulkMessageCallback::Callback() {
        se->Log() << "BulkMessageCallback" << LogLevel::Info;
    }
} // namespace statusengine
