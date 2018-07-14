#pragma once

#include <map>
#include <string>
#include <vector>

#include "LogStream.h"
#include "Nebmodule.h"

#include "EventCallback/BulkMessageCallback.h"
#include "NagiosCallbacks/NebmoduleCallback.h"

namespace statusengine {
    class Configuration;
    class MessageHandlerList;
    class Nebmodule;

    class Statusengine {
        friend class Nebmodule;

      public:
        Statusengine(const Statusengine &Statusengine) = delete;
        Statusengine(Statusengine &&Statusengine) = delete;
        Statusengine &operator=(const Statusengine &) = delete;

        int Init();
        void InitEventCallbacks();

        LogStream &Log();
        void FlushBulkQueue();
        void RegisterCallback(NebmoduleCallback *cb);
        void RegisterEventCallback(EventCallback *ecb);
        MessageHandlerList *GetMessageHandler() const;

      private:
        Statusengine(nebmodule *handle, std::string configurationPath);
        ~Statusengine();

        int Callback(int event_type, void *data);

        void SetModuleInfo(int modinfo, std::string text);

        nebmodule *nebhandle;
        std::string configurationPath;
        Configuration *configuration;
        MessageHandlerList *messageHandler;
        LogStream *ls;
        std::map<NEBCallbackType, std::vector<NebmoduleCallback *> *> *callbacks;
        BulkMessageCallback *bulkCallback;
    };
} // namespace statusengine
