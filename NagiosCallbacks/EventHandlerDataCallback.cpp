#include "EventHandlerDataCallback.h"

#include "NagiosObjects/NagiosEventHandlerData.h"
#include "Statusengine.h"

namespace statusengine {
    EventHandlerDataCallback::EventHandlerDataCallback(Statusengine *se)
        : NebmoduleCallback(NEBCALLBACK_EVENT_HANDLER_DATA, se) {}

    void
    EventHandlerDataCallback::Callback(int event_type,
                                       nebstruct_event_handler_data *data) {
        auto myData = NagiosEventHandlerData(data);
        se->SendMessage("statusngin_eventhandler", myData.GetData().dump());
    }
} // namespace statusengine
