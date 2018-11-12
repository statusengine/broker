#pragma once

#include <map>
#include <string>
#include <memory>

#include "LogStream.h"
#include "Nebmodule.h"

#include "EventCallback/BulkMessageCallback.h"
#include "EventCallback/MessageWorkerCallback.h"
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
        void RegisterEventCallback(EventCallback *ecb);
        MessageHandlerList *GetMessageHandler() const;

        template<typename T, typename... _Args>
        void RegisterCallback(_Args&&... __args) {
            std::shared_ptr<NebmoduleCallback> cb  = std::static_pointer_cast<NebmoduleCallback>(std::make_shared<T>(std::forward<_Args>(__args)...));
            NEBCallbackType cbType = cb->GetCallbackType();
            if (callbacks.find(cbType) == callbacks.end())
                Nebmodule::RegisterCallback(cbType);
            callbacks.insert(std::make_pair(cbType, cb));
        }

      private:
        Statusengine(nebmodule *handle, std::string configurationPath);
        ~Statusengine();

        int Callback(int event_type, void *data);

        void SetModuleInfo(int modinfo, std::string text);

        nebmodule *nebhandle;
        std::string configurationPath;
        Configuration *configuration;
        MessageHandlerList *messageHandler;
        LogStream ls;
        std::multimap<NEBCallbackType, std::shared_ptr<NebmoduleCallback>> callbacks;
        BulkMessageCallback *bulkCallback;
        MessageWorkerCallback *messageWorkerCallback;
    };
} // namespace statusengine
