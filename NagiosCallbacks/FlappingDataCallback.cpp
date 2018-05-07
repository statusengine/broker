#include "FlappingDataCallback.h"

#include "NagiosObjects/NagiosFlappingData.h"
#include "Statusengine.h"

namespace statusengine {
    FlappingDataCallback::FlappingDataCallback(Statusengine *se) : NebmoduleCallback(NEBCALLBACK_FLAPPING_DATA, se) {}

    void FlappingDataCallback::Callback(int event_type, nebstruct_flapping_data *data) {
        auto myData = NagiosFlappingData(data);
        se->SendMessage("statusngin_flappings", myData.ToString());
    }
} // namespace statusengine
