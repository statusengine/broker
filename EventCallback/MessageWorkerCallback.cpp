#include "MessageWorkerCallback.h"

#include "MessageHandler/MessageHandlerList.h"
#include "Statusengine.h"

namespace statusengine {
    MessageWorkerCallback::MessageWorkerCallback(Statusengine *se, double interval)
        : EventCallback(se), interval(interval) {}

    double MessageWorkerCallback::Interval() {
        return interval;
    }
    void MessageWorkerCallback::Callback() {
        se->GetMessageHandler()->Worker();
    }
} // namespace statusengine
