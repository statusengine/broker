#include "Configuration.h"

#include <list>

#include "Statusengine.h"


namespace statusengine {

	Configuration::Configuration(Statusengine *se, const toml::Table &cfg) : se(se) {
		toml::Table queues;
		try {
			queues = cfg.at("Queues").cast<toml::value_t::Table>();
		}
		catch (std::out_of_range &oor) {
		}
		catch (const toml::type_error &tte) {
			se->Log() << "Invalid configuration: Queues isn't a table! Ignoring for now..." << eoem;
		}

		SetQueueHostStatus(GetIgnore(queues, "HostStatus", false));
		SetQueueHostCheck(GetIgnore(queues, "HostCheck", false));
		SetQueueOCHP(GetIgnore(queues, "OCHP", false));
		SetQueueServiceStatus(GetIgnore(queues, "ServiceStatus", false));
		SetQueueServiceCheck(GetIgnore(queues, "ServiceCheck", false));
		SetQueueServicePerfData(GetIgnore(queues, "ServicePerfData", false));
		SetQueueOCSP(GetIgnore(queues, "OCSP", false));
		SetQueueStateChange(GetIgnore(queues, "StateChange", false));
		SetQueueLogData(GetIgnore(queues, "LogData", false));
		SetQueueSystemCommandData(GetIgnore(queues, "SystemCommandData", false));
		SetQueueCommentData(GetIgnore(queues, "CommentData", false));
		SetQueueExternalCommandData(GetIgnore(queues, "ExternalCommandData", false));
		SetQueueAcknowledgementData(GetIgnore(queues, "AcknowledgementData", false));
		SetQueueFlappingData(GetIgnore(queues, "FlappingData", false));
		SetQueueDowntimeData(GetIgnore(queues, "DowntimeData", false));
		SetQueueNotificationData(GetIgnore(queues, "NotificationData", false));
		SetQueueProgramStatusData(GetIgnore(queues, "ProgramStatusData", false));
		SetQueueContactStatusData(GetIgnore(queues, "ContactStatusData", false));
		SetQueueContactNotificationData(GetIgnore(queues, "ContactNotificationData", false));
		SetQueueContactNotificationMethodData(GetIgnore(queues, "ContactNotificationMethodData", false));
		SetQueueEventHandlerData(GetIgnore(queues, "EventHandlerData", false));
		SetQueueProcessData(GetIgnore(queues, "ProcessData", false));

		toml::Table gearman;
		try {
			gearman = cfg.at("Gearman").cast<toml::value_t::Table>();
		}
		catch (const std::out_of_range &oor) {
		}
		catch (const toml::type_error &tte) {
			se->Log() << "Invalid configuration: Gearman isn't a table! Ignoring for now..." << eoem;
		}

		std::list<std::string> servers;
		try {
			servers = toml::get<std::list<std::string>>(gearman.at("Servers"));
		}
		catch (std::out_of_range &oor) {
		}
		catch (const toml::type_error &tte) {
			se->Log() << "Invalid configuration: Gearman::Servers isn't an array! Ignoring for now..." << eoem;
		}

		ResetGearman();
		if (!servers.empty()) {
			for (auto it = servers.begin(); it != servers.end(); ++it) {
				AddGearman(*it);
			}
		}
	}

	bool Configuration::GetQueueHostStatus() const {
		return HostStatus;
	}

	void Configuration::SetQueueHostStatus(const bool v) {
		HostStatus = v;
	}

	bool Configuration::GetQueueHostCheck() const {
		return HostCheck;
	}

	void Configuration::SetQueueHostCheck(const bool v) {
		HostCheck = v;
	}

	bool Configuration::GetQueueOCHP() const {
		return OCHP;
	}

	void Configuration::SetQueueOCHP(const bool v) {
		OCHP = v;
	}

	bool Configuration::GetQueueServiceStatus() const {
		return ServiceStatus;
	}

	void Configuration::SetQueueServiceStatus(const bool v) {
		ServiceStatus = v;
	}

	bool Configuration::GetQueueServiceCheck() const {
		return ServiceCheck;
	}

	void Configuration::SetQueueServiceCheck(const bool v) {
		ServiceCheck = v;
	}

	bool Configuration::GetQueueServicePerfData() const {
		return ServicePerfData;
	}

	void Configuration::SetQueueServicePerfData(const bool v) {
		ServicePerfData = v;
	}

	bool Configuration::GetQueueOCSP() const {
		return OCSP;
	}

	void Configuration::SetQueueOCSP(const bool v) {
		OCSP = v;
	}

	bool Configuration::GetQueueStateChange() const {
		return StateChange;
	}

	void Configuration::SetQueueStateChange(const bool v) {
		StateChange = v;
	}

	bool Configuration::GetQueueLogData() const {
		return LogData;
	}

	void Configuration::SetQueueLogData(const bool v) {
		LogData = v;
	}

	bool Configuration::GetQueueSystemCommandData() const {
		return SystemCommandData;
	}

	void Configuration::SetQueueSystemCommandData(const bool v) {
		SystemCommandData = v;
	}

	bool Configuration::GetQueueCommentData() const {
		return CommentData;
	}

	void Configuration::SetQueueCommentData(const bool v) {
		CommentData = v;
	}

	bool Configuration::GetQueueExternalCommandData() const {
		return ExternalCommandData;
	}

	void Configuration::SetQueueExternalCommandData(const bool v) {
		ExternalCommandData = v;
	}

	bool Configuration::GetQueueAcknowledgementData() const {
		return AcknowledgementData;
	}

	void Configuration::SetQueueAcknowledgementData(const bool v) {
		AcknowledgementData = v;
	}

	bool Configuration::GetQueueFlappingData() const {
		return FlappingData;
	}

	void Configuration::SetQueueFlappingData(const bool v) {
		FlappingData = v;
	}

	bool Configuration::GetQueueDowntimeData() const {
		return DowntimeData;
	}

	void Configuration::SetQueueDowntimeData(const bool v) {
		DowntimeData = v;
	}

	bool Configuration::GetQueueNotificationData() const {
		return NotificationData;
	}

	void Configuration::SetQueueNotificationData(const bool v) {
		NotificationData = v;
	}

	bool Configuration::GetQueueProgramStatusData() const {
		return ProgramStatusData;
	}

	void Configuration::SetQueueProgramStatusData(const bool v) {
		ProgramStatusData = v;
	}

	bool Configuration::GetQueueContactStatusData() const {
		return ContactStatusData;
	}

	void Configuration::SetQueueContactStatusData(const bool v) {
		ContactStatusData = v;
	}

	bool Configuration::GetQueueContactNotificationData() const {
		return ContactNotificationData;
	}

	void Configuration::SetQueueContactNotificationData(const bool v) {
		ContactNotificationData = v;
	}

	bool Configuration::GetQueueContactNotificationMethodData() const {
		return ContactNotificationMethodData;
	}

	void Configuration::SetQueueContactNotificationMethodData(const bool v) {
		ContactNotificationMethodData = v;
	}

	bool Configuration::GetQueueEventHandlerData() const {
		return EventHandlerData;
	}

	void Configuration::SetQueueEventHandlerData(const bool v) {
		EventHandlerData = v;
	}

	bool Configuration::GetQueueProcessData() const {
		return ProcessData;
	}

	void Configuration::SetQueueProcessData(const bool v) {
		ProcessData = v;
	}

	bool Configuration::GetQueueRestartData() const {
		return RestartData;
	}

	void Configuration::SetQueueRestartData(const bool v) {
		RestartData = v;
	}

	void Configuration::ResetGearman() {
		gearmanUrls.clear();
		gearmanClients.clear();
	}

	void Configuration::AddGearman(const std::string &url) {
		gearmanUrls.push_back(url);
	}

	const std::list<std::string>& Configuration::GetGearmanList() const {
		return gearmanUrls;
	}

	std::list<std::shared_ptr<GearmanClient>> Configuration::GetGearmanClients(Statusengine *se) {
		if (gearmanUrls.size() != 0 && gearmanClients.size() == 0) {
			for (auto it = gearmanUrls.begin(); it != gearmanUrls.end(); ++it) {
				gearmanClients.push_back(std::make_shared<GearmanClient>(se->Log(), *it));
			}
		}
		return gearmanClients;
	}

}
