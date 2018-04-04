#include "Configuration.h"

#include "Statusengine.h"


namespace statusengine {

	Configuration::Configuration() {

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

	void to_json(json& j, const Configuration& c) {
		json queues;
		queues["HostStatus"] = c.GetQueueHostStatus();
		queues["HostCheck"] = c.GetQueueHostCheck();
		queues["OCHP"] = c.GetQueueOCHP();
		queues["ServiceStatus"] = c.GetQueueServiceStatus();
		queues["ServiceCheck"] = c.GetQueueServiceCheck();
		queues["ServicePerfData"] = c.GetQueueServicePerfData();
		queues["OCSP"] = c.GetQueueOCSP();
		queues["StateChange"] = c.GetQueueStateChange();
		queues["LogData"] = c.GetQueueLogData();
		queues["SystemCommandData"] = c.GetQueueSystemCommandData();
		queues["CommentData"] = c.GetQueueCommentData();
		queues["ExternalCommandData"] = c.GetQueueExternalCommandData();
		queues["AcknowledgementData"] = c.GetQueueAcknowledgementData();
		queues["FlappingData"] = c.GetQueueFlappingData();
		queues["DowntimeData"] = c.GetQueueDowntimeData();
		queues["NotificationData"] = c.GetQueueNotificationData();
		queues["ProgramStatusData"] = c.GetQueueProgramStatusData();
		queues["ContactStatusData"] = c.GetQueueContactStatusData();
		queues["ContactNotificationData"] = c.GetQueueContactNotificationData();
		queues["ContactNotificationMethodData"] = c.GetQueueContactNotificationMethodData();
		queues["EventHandlerData"] = c.GetQueueEventHandlerData();
		queues["ProcessData"] = c.GetQueueProcessData();
		j["Queues"] = queues;

		json gearman;

		auto gearmanUrls = c.GetGearmanList();
		for (auto it = gearmanUrls.begin(); it != gearmanUrls.end(); ++it) {
			gearman.push_back(*it);
		}

		j["Gearman"] = gearman;
	}

	template<typename T>
	T get_json(const json& j, const char* name, T defaultValue) {
		try {
			return j.at(name);
		}
		catch (const nlohmann::detail::out_of_range &oor) {
		}
		return defaultValue;
	}

	void from_json(const json& j, Configuration& c) {
		json queues;
		try {
			queues = j.at("Queues");
		}
		catch (const nlohmann::detail::out_of_range &oor) {
		}

		c.SetQueueHostStatus(get_json<bool>(queues, "HostStatus", false));
		c.SetQueueHostCheck(get_json<bool>(queues, "HostCheck", false));
		c.SetQueueOCHP(get_json<bool>(queues, "OCHP", false));
		c.SetQueueServiceStatus(get_json<bool>(queues, "ServiceStatus", false));
		c.SetQueueServiceCheck(get_json<bool>(queues, "ServiceCheck", false));
		c.SetQueueServicePerfData(get_json<bool>(queues, "ServicePerfData", false));
		c.SetQueueOCSP(get_json<bool>(queues, "OCSP", false));
		c.SetQueueStateChange(get_json<bool>(queues, "StateChange", false));
		c.SetQueueLogData(get_json<bool>(queues, "LogData", false));
		c.SetQueueSystemCommandData(get_json<bool>(queues, "SystemCommandData", false));
		c.SetQueueCommentData(get_json<bool>(queues, "CommentData", false));
		c.SetQueueExternalCommandData(get_json<bool>(queues, "ExternalCommandData", false));
		c.SetQueueAcknowledgementData(get_json<bool>(queues, "AcknowledgementData", false));
		c.SetQueueFlappingData(get_json<bool>(queues, "FlappingData", false));
		c.SetQueueDowntimeData(get_json<bool>(queues, "DowntimeData", false));
		c.SetQueueNotificationData(get_json<bool>(queues, "NotificationData", false));
		c.SetQueueProgramStatusData(get_json<bool>(queues, "ProgramStatusData", false));
		c.SetQueueContactStatusData(get_json<bool>(queues, "ContactStatusData", false));
		c.SetQueueContactNotificationData(get_json<bool>(queues, "ContactNotificationData", false));
		c.SetQueueContactNotificationMethodData(get_json<bool>(queues, "ContactNotificationMethodData", false));
		c.SetQueueEventHandlerData(get_json<bool>(queues, "EventHandlerData", false));
		c.SetQueueProcessData(get_json<bool>(queues, "ProcessData", false));

		json gearman;
		try {
			gearman = j.at("Gearman");
		}
		catch (const nlohmann::detail::out_of_range &oor) {
		}

		c.ResetGearman();
		if (!gearman.empty()) {
			for (json::iterator it = gearman.begin(); it != gearman.end(); ++it) {
				c.AddGearman(*it);
			}
		}
	}
}
