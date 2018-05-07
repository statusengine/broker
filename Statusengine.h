#ifndef STATUSENGINE_H
#define STATUSENGINE_H

#include <list>
#include <memory>
#include <sstream>
#include <string>

#include "nebmodule.h"

#include "GearmanClient.h"
#include "NagiosCallbacks/AcknowledgementDataCallback.h"
#include "NagiosCallbacks/CommentDataCallback.h"
#include "NagiosCallbacks/ContactNotificationDataCallback.h"
#include "NagiosCallbacks/ContactNotificationMethodDataCallback.h"
#include "NagiosCallbacks/ContactStatusDataCallback.h"
#include "NagiosCallbacks/DowntimeDataCallback.h"
#include "NagiosCallbacks/EventHandlerDataCallback.h"
#include "NagiosCallbacks/ExternalCommandDataCallback.h"
#include "NagiosCallbacks/FlappingDataCallback.h"
#include "NagiosCallbacks/HostCheckCallback.h"
#include "NagiosCallbacks/HostStatusCallback.h"
#include "NagiosCallbacks/LogDataCallback.h"
#include "NagiosCallbacks/NotificationDataCallback.h"
#include "NagiosCallbacks/ProcessDataCallback.h"
#include "NagiosCallbacks/ProgramStatusDataCallback.h"
#include "NagiosCallbacks/ServiceCheckCallback.h"
#include "NagiosCallbacks/ServiceStatusCallback.h"
#include "NagiosCallbacks/StateChangeCallback.h"
#include "NagiosCallbacks/SystemCommandDataCallback.h"

namespace statusengine {
    class Configuration;

    class Statusengine {
      public:
        Statusengine(nebmodule *handle, std::string configurationPath);
        ~Statusengine();

        int Init();

        std::stringstream Log();
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

#endif // !STATUSENGINE_H