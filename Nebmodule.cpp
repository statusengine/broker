#include "Nebmodule.h"

#include <string>

#include "Statusengine.h"

// This is required by naemon
extern "C" {
NEB_API_VERSION(CURRENT_NEB_API_VERSION);
}

namespace statusengine {
    Statusengine *Nebmodule::se = nullptr;

    int Nebmodule::Init(nebmodule *handle, std::string args) {
        se = new Statusengine(handle, args);
        return se->Init();
    }

    int Nebmodule::Deinit(int reason) {
        delete se;
        return 0;
    }

    int Nebmodule::Callback(int event_type, void *data) {
        return se->Callback(event_type, data);
    }

    bool Nebmodule::RegisterCallback(NEBCallbackType cbType) {
        int result = neb_register_callback(cbType, se->nebhandle, 0, nebmodule_callback);

        if (result != 0) {
            se->Log() << "Could not register nebmodule_callback for Event Type " << cbType << ": " << result
                      << LogLevel::Error;
            return false;
        }
        else {
            se->Log() << "Register nebmodule_callback for Event Type: " << cbType << LogLevel::Info;
            return true;
        }
    }
} // namespace statusengine

extern "C" int nebmodule_init(int flags, char *args, nebmodule *handle) {
    return statusengine::Nebmodule::Init(handle, std::string(args));
}

extern "C" int nebmodule_deinit(int flags, int reason) {
    return statusengine::Nebmodule::Deinit(reason);
}

int nebmodule_callback(int event_type, void *data) {
    return statusengine::Nebmodule::Callback(event_type, data);
}
