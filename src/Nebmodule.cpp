#include "Nebmodule.h"

#include <string>
#include <iconv.h>
#include <memory>

#include "Statusengine.h"
#include "Exceptions.h"

// This is required by naemon
extern "C" {
NEB_API_VERSION(CURRENT_NEB_API_VERSION)
}

namespace statusengine {

    Nebmodule::Nebmodule(Neb_NebmodulePtr handle, std::string args) : handle(handle) {
        uc = uchardet_new();
        se = new Statusengine(*this, std::move(args));
    }

    Nebmodule::~Nebmodule() {
        delete se;
        uchardet_delete(uc);
        uc = nullptr;
    }

    void Nebmodule::Init() {
        se->Init();
    }

    int Nebmodule::Callback(int event_type, void *data) {
        return se->Callback(event_type, data);
    }

    bool Nebmodule::RegisterCallback(NEBCallbackType cbType) {
        int result = neb_register_callback(cbType, handle, 0, nebmodule_callback);

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

    std::string Nebmodule::EncodeString(char *inputData) {
        if(inputData == nullptr) {
            return std::string();
        }
        auto lendata = std::strlen(inputData); // we can't use strnlen here, we don't have any idea of the length here...
        uchardet_handle_data(uc, inputData, lendata); //TODO error handling
        uchardet_data_end(uc);
        auto charset = uchardet_get_charset(uc);
        uchardet_reset(uc);

        if(std::strcmp(charset, "UTF-8")) {
            // We don't have to convert it, if it is already UTF-8
            return std::string(inputData, lendata);
        }
        auto outputDataLength = lendata*4;
        char *outputData = new char[outputDataLength]; // utf-8 possibly needs up to 4 bytes for a single character :/
        
        auto cd = iconv_open("UTF-8", charset);
        auto outputLength = iconv(cd, &inputData, &lendata, &outputData, &outputDataLength);
        std::string result(outputData, outputLength);
        iconv_close(cd);

        delete [] outputData;
        delete [] charset;

        return result;
    }

    Neb_NebmodulePtr Nebmodule::GetNebNebmodulePtr() {
        return handle;
    }
} // namespace statusengine

static statusengine::INebmodulePtr instance;

extern "C" int nebmodule_init(int, char *args, nebmodule *handle) {
    try {
        instance = std::make_shared<statusengine::Nebmodule>(handle, std::string(args));
        instance->Init();
        return 0;
    } catch (const statusengine::StatusengineException &e) {
        return 1;
    }
}

extern "C" int nebmodule_deinit(int, int) {
    try {
        instance.reset();
        return 0;
    } catch (const statusengine::StatusengineException &e) {
        return 1;
    }
}

int nebmodule_callback(int event_type, void *data) {
    return instance->Callback(event_type, data);
}
