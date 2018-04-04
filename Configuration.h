#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <list>

#include "vendor/json.hpp"

#include "GearmanClient.h"


using json = nlohmann::json;


namespace statusengine {

	class Statusengine;

	class Configuration {
	public:
		Configuration();

		bool GetQueueHostStatus() const;
		void SetQueueHostStatus(const bool v);
		bool GetQueueHostCheck() const;
		void SetQueueHostCheck(const bool v);
		bool GetQueueOCHP() const;
		void SetQueueOCHP(const bool v);
		bool GetQueueServiceStatus() const;
		void SetQueueServiceStatus(const bool v);
		bool GetQueueServiceCheck() const;
		void SetQueueServiceCheck(const bool v);
		bool GetQueueServicePerfData() const;
		void SetQueueServicePerfData(const bool v);
		bool GetQueueOCSP() const;
		void SetQueueOCSP(const bool v);
		bool GetQueueStateChange() const;
		void SetQueueStateChange(const bool v);
		bool GetQueueLogData() const;
		void SetQueueLogData(const bool v);
		bool GetQueueSystemCommandData() const;
		void SetQueueSystemCommandData(const bool v);
		bool GetQueueCommentData() const;
		void SetQueueCommentData(const bool v);
		bool GetQueueExternalCommandData() const;
		void SetQueueExternalCommandData(const bool v);
		bool GetQueueAcknowledgementData() const;
		void SetQueueAcknowledgementData(const bool v);
		bool GetQueueFlappingData() const;
		void SetQueueFlappingData(const bool v);
		bool GetQueueDowntimeData() const;
		void SetQueueDowntimeData(const bool v);
		bool GetQueueNotificationData() const;
		void SetQueueNotificationData(const bool v);
		bool GetQueueProgramStatusData() const;
		void SetQueueProgramStatusData(const bool v);
		bool GetQueueContactStatusData() const;
		void SetQueueContactStatusData(const bool v);
		bool GetQueueContactNotificationData() const;
		void SetQueueContactNotificationData(const bool v);
		bool GetQueueContactNotificationMethodData() const;
		void SetQueueContactNotificationMethodData(const bool v);
		bool GetQueueEventHandlerData() const;
		void SetQueueEventHandlerData(const bool v);
		bool GetQueueProcessData() const;
		void SetQueueProcessData(const bool v);
		bool GetQueueRestartData() const;
		void SetQueueRestartData(const bool v);

		void ResetGearman();
		void AddGearman(const std::string &url);
		const std::list<std::string>& GetGearmanList() const;
		std::list<std::shared_ptr<GearmanClient>> GetGearmanClients(Statusengine *se);
	private:
		bool HostStatus = false;
		bool HostCheck = false;
		bool OCHP = false;
		bool ServiceStatus = false;
		bool ServiceCheck = false;
		bool ServicePerfData = false;
		bool OCSP = false;
		bool StateChange = false;
		bool LogData = false;
		bool SystemCommandData = false;
		bool CommentData = false;
		bool ExternalCommandData = false;
		bool AcknowledgementData = false;
		bool FlappingData = false;
		bool DowntimeData = false;
		bool NotificationData = false;
		bool ProgramStatusData = false;
		bool ContactStatusData = false;
		bool ContactNotificationData = false;
		bool ContactNotificationMethodData = false;
		bool EventHandlerData = false;
		bool ProcessData = false;
		bool RestartData = false;

		std::list<std::string> gearmanUrls;

		std::list<std::shared_ptr<GearmanClient>> gearmanClients;
	};

	void to_json(json& j, const Configuration& c);
	void from_json(const json& j, Configuration& c);
}

#endif // !CONFIGURATION_H
