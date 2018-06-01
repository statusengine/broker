#pragma once

#include <string>

#include "LogStream.h"
#include "nebmodule.h"

#include "MessageHandler/MessageHandlerList.h"

#include "NagiosCallbacks/NagiosCallbacks.h"

namespace statusengine {
    class Configuration;

    class Statusengine {
      public:
        Statusengine(nebmodule *handle, std::string configurationPath);
        ~Statusengine();
        Statusengine(const Statusengine &Statusengine) = delete;
        Statusengine(Statusengine &&Statusengine) = delete;
        Statusengine &operator=(const Statusengine &) = delete;

        int Init();

        LogStream &Log();
        void SendMessage(const std::string queue, const std::string message) const;

        template <typename T> void RegisterCallback(NebmoduleCallback<T> *cb) {
            RegisterCallback(cb->GetCallbackType(), cb->GetCallbackFunction(), cb->GetPriority());
        };

      private:
        void SetModuleInfo(int modinfo, std::string text);
        void RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority = 0);
        // void DeregisterCallback(NEBCallbackType type, int callback(int, void
        // *));

        nebmodule *nebhandle;
        std::string configurationPath;
        Configuration *configuration;
        MessageHandlerList *messageHandlers;
        LogStream *ls;

        HostStatusCallback *cbHostStatus;
        HostCheckCallback *cbHostCheck;
        ServiceStatusCallback *cbServiceStatus;
        ServiceCheckCallback *cbServiceCheck;
        StateChangeCallback *cbStateChange;
        LogDataCallback *cbLogData;
        SystemCommandDataCallback *cbSystemCommandData;
        CommentDataCallback *cbCommentData;
        ExternalCommandDataCallback *cbExternalCommandData;
        AcknowledgementDataCallback *cbAcknowledgementData;
        FlappingDataCallback *cbFlappingData;
        DowntimeDataCallback *cbDowntimeData;
        NotificationDataCallback *cbNotificationData;
        ProgramStatusDataCallback *cbProgramStatusData;
        ContactStatusDataCallback *cbContactStatusData;
        ContactNotificationDataCallback *cbContactNotificationData;
        ContactNotificationMethodDataCallback *cbContactNotificationMethodData;
        EventHandlerDataCallback *cbEventHandlerData;
        ProcessDataCallback *cbProcessData;
    };
} // namespace statusengine
