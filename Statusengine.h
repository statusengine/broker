#ifndef STATUSENGINE_H
#define STATUSENGINE_H

#include <sstream>
#include <string>

#include "nebmodule.h"

#include "GearmanClient.h"
#include "Configuration.h"
#include "NagiosCallbacks/HostStatusCallback.h"
#include "NagiosCallbacks/HostCheckCallback.h"
#include "NagiosCallbacks/ServiceStatusCallback.h"
#include "NagiosCallbacks/ServiceCheckCallback.h"
#include "NagiosCallbacks/StateChangeCallback.h"
#include "NagiosCallbacks/LogDataCallback.h"
#include "NagiosCallbacks/SystemCommandDataCallback.h"
#include "NagiosCallbacks/CommentDataCallback.h"
#include "NagiosCallbacks/ExternalCommandDataCallback.h"
#include "NagiosCallbacks/AcknowledgementDataCallback.h"
#include "NagiosCallbacks/FlappingDataCallback.h"
#include "NagiosCallbacks/DowntimeDataCallback.h"
#include "NagiosCallbacks/NotificationDataCallback.h"
#include "NagiosCallbacks/ProgramStatusDataCallback.h"
#include "NagiosCallbacks/ContactStatusDataCallback.h"
#include "NagiosCallbacks/ContactNotificationDataCallback.h"
#include "NagiosCallbacks/ContactNotificationMethodDataCallback.h"
#include "NagiosCallbacks/EventHandlerDataCallback.h"
#include "NagiosCallbacks/ProcessDataCallback.h"


namespace statusengine {
	class Statusengine {
	public:
		Statusengine(nebmodule *handle);
		~Statusengine();

		std::ostream& Log();
		void SendMessage(const std::string queue, const std::string message) const;

		template<typename T>
		void RegisterCallback(NebmoduleCallback<T> *cb) {
			RegisterCallback(cb->GetCallbackType(), cb->GetCallbackFunction(), cb->GetPriority());
		};

	private:
		void SetModuleInfo(int modinfo, std::string text);
		void RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority = 0);
		//void DeregisterCallback(NEBCallbackType type, int callback(int, void *));
		
		nebmodule *nebhandle;
		std::ostringstream ls; // logging
		Configuration configuration;
		GearmanClient *gearman;

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

		std::list<std::shared_ptr<GearmanClient>> gearmanClients;
	};
}

#endif // !STATUSENGINE_H