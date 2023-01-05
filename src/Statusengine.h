#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>

#include "Nebmodule.h"
#include "IStatusengine.h"

#include "EventCallback.h"
#include "NebmoduleCallback.h"
#include "Configuration.h"

namespace statusengine {
    class Nebmodule;

    class Statusengine : public IStatusengine {
        friend class Nebmodule;

      public:
        Statusengine(const Statusengine &Statusengine) = delete;
        Statusengine(Statusengine &&Statusengine) = delete;
        Statusengine &operator=(const Statusengine &) = delete;

        int Init();
        void InitEventCallbacks() override;

        LogStream &Log() override;
        void FlushBulkQueue() override;
        void RegisterEventCallback(EventCallback *ecb);
        IMessageHandlerList *GetMessageHandler() const override;

        template<typename T>
        void RegisterCallback(const std::vector<Queue> &queueList) {
            bool queueExists = false;
            for (auto q : queueList) {
                if(messageHandler->QueueExists(q)) {
                    queueExists = true;
                    break;
                }
            }
            if (queueExists) {
                RegisterCallback<T>();
            }
        }

        template<typename T>
        void RegisterCallback() {
            auto cb  = new T(*this);
            NEBCallbackType cbType = cb->GetCallbackType();
            if (callbacks.find(cbType) == callbacks.end())
                neb.RegisterCallback(cbType);
            callbacks.insert(std::make_pair(cbType, std::unique_ptr<NebmoduleCallback>(cb)));
        }

        time_t GetStartupScheduleMax() const override {
            return configuration->GetStartupScheduleMax();
        }

        INebmodule& GetNebmodule() override {
            return neb;
        }

      private:
        Statusengine(INebmodule &neb, std::string configurationPath);
        ~Statusengine();

        int Callback(int event_type, void *data);

        void SetModuleInfo(int modinfo, const std::string &text);

        INebmodule &neb;
        std::string configurationPath;
        Configuration *configuration;
        IMessageHandlerList *messageHandler;
        LogStream ls;
        std::multimap<NEBCallbackType, std::unique_ptr<NebmoduleCallback>> callbacks;
        BulkMessageCallback *bulkCallback;
        MessageWorkerCallback *messageWorkerCallback;
    };
} // namespace statusengine
