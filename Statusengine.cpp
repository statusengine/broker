#include "Statusengine.h"

#include <sstream>
#include <memory>
#include <fstream>

#include "vendor/json.hpp"

#include "LogStream.h"

#include "Configuration.h"


using json = nlohmann::json;

namespace statusengine {

	Statusengine::Statusengine(nebmodule *handle, std::string configurationPath) :
		nebhandle(handle), configurationPath(configurationPath) {
	}

	int Statusengine::Init() {
		SetModuleInfo(NEBMODULE_MODINFO_TITLE, "Statusengine - the missing event broker");
		SetModuleInfo(NEBMODULE_MODINFO_AUTHOR, "Johannes Drummer");
		SetModuleInfo(NEBMODULE_MODINFO_TITLE, "Copyright (c) 2018 - present Johannes Drummer");
		SetModuleInfo(NEBMODULE_MODINFO_VERSION, "4.0.0");
		SetModuleInfo(NEBMODULE_MODINFO_LICENSE, "GPL v2");
		SetModuleInfo(NEBMODULE_MODINFO_DESC, "A powerful and flexible event broker");

		ls << "the missing event broker" << eom;
		ls << "This is the c++ version of statusengine event broker" << eom;

		std::ifstream cfgFile = std::ifstream(configurationPath, std::fstream::in);
		json j;
		try {
			cfgFile >> j;
		}
		catch (nlohmann::detail::parse_error &pe) {
			ls << "Invalid json configuration file" << eoem;
			return 1;
		}

		configuration = j;
		cfgFile.close();

		gearmanClients = configuration.GetGearmanClients(this);

		if (configuration.GetQueueHostStatus()) {
			cbHostStatus = new HostStatusCallback(this);
			RegisterCallback(cbHostStatus);
		}

		if (configuration.GetQueueHostCheck() || configuration.GetQueueOCHP()) {
			cbHostCheck = new HostCheckCallback(this, configuration.GetQueueHostCheck(), configuration.GetQueueOCHP());
			RegisterCallback(cbHostCheck);
		}

		if (configuration.GetQueueServiceStatus()) {
			cbServiceStatus = new ServiceStatusCallback(this);
			RegisterCallback(cbServiceStatus);
		}

		if (configuration.GetQueueServiceCheck() || configuration.GetQueueOCSP() || configuration.GetQueueServicePerfData()) {
			cbServiceCheck = new ServiceCheckCallback(this, configuration.GetQueueServiceCheck(), configuration.GetQueueOCSP(), configuration.GetQueueServicePerfData());
			RegisterCallback(cbServiceCheck);
		}

		if (configuration.GetQueueStateChange()) {
			cbStateChange = new StateChangeCallback(this);
			RegisterCallback(cbStateChange);
		}

		if (configuration.GetQueueLogData()) {
			cbLogData = new LogDataCallback(this);
			RegisterCallback(cbLogData);
		}

		if (configuration.GetQueueSystemCommandData()) {
			cbSystemCommandData = new SystemCommandDataCallback(this);
			RegisterCallback(cbSystemCommandData);
		}

		if (configuration.GetQueueCommentData()) {
			cbCommentData = new CommentDataCallback(this);
			RegisterCallback(cbCommentData);
		}

		if (configuration.GetQueueExternalCommandData()) {
			cbExternalCommandData = new ExternalCommandDataCallback(this);
			RegisterCallback(cbExternalCommandData);
		}

		if (configuration.GetQueueAcknowledgementData()) {
			cbAcknowledgementData = new AcknowledgementDataCallback(this);
			RegisterCallback(cbAcknowledgementData);
		}

		if (configuration.GetQueueFlappingData()) {
			cbFlappingData = new FlappingDataCallback(this);
			RegisterCallback(cbFlappingData);
		}

		if (configuration.GetQueueDowntimeData()) {
			cbDowntimeData = new DowntimeDataCallback(this);
			RegisterCallback(cbDowntimeData);
		}

		if (configuration.GetQueueNotificationData()) {
			cbNotificationData = new NotificationDataCallback(this);
			RegisterCallback(cbNotificationData);
		}

		if (configuration.GetQueueProgramStatusData()) {
			cbProgramStatusData = new ProgramStatusDataCallback(this);
			RegisterCallback(cbProgramStatusData);
		}

		if (configuration.GetQueueContactStatusData()) {
			cbContactStatusData = new ContactStatusDataCallback(this);
			RegisterCallback(cbContactStatusData);
		}

		if (configuration.GetQueueContactNotificationData()) {
			cbContactNotificationData = new ContactNotificationDataCallback(this);
			RegisterCallback(cbContactNotificationData);
		}

		if (configuration.GetQueueContactNotificationMethodData()) {
			cbContactNotificationMethodData = new ContactNotificationMethodDataCallback(this);
			RegisterCallback(cbContactNotificationMethodData);
		}

		if (configuration.GetQueueEventHandlerData()) {
			cbEventHandlerData = new EventHandlerDataCallback(this);
			RegisterCallback(cbEventHandlerData);
		}

		if (configuration.GetQueueRestartData() || configuration.GetQueueProcessData()) {
			cbProcessData = new ProcessDataCallback(this, configuration.GetQueueRestartData(), configuration.GetQueueProcessData());
			RegisterCallback(cbProcessData);
		}

		return 0;
	}
	
	Statusengine::~Statusengine() {
		ls << "unload" << eom;
		neb_deregister_module_callbacks(nebhandle);
		ls << "unload finished" << eom;
		delete gearman;

		delete cbHostStatus;
		delete cbHostCheck;
		delete cbServiceStatus;
		delete cbServiceCheck;
		delete cbStateChange;
		delete cbLogData;
		delete cbSystemCommandData;
		delete cbCommentData;
		delete cbExternalCommandData;
		delete cbAcknowledgementData;
		delete cbFlappingData;
		delete cbDowntimeData;
		delete cbNotificationData;
		delete cbProgramStatusData;
		delete cbContactStatusData;
		delete cbContactNotificationData;
		delete cbContactNotificationMethodData;
		delete cbEventHandlerData;
		delete cbProcessData;
	}

	std::ostream& Statusengine::Log() {
		return ls;
	}

	void Statusengine::SendMessage(const std::string queue, const std::string message) const {
		for (auto it = gearmanClients.begin(); it != gearmanClients.end(); ++it) {
			(*it)->SendMessage(queue, message);
		}
	}

	void Statusengine::SetModuleInfo(int modinfo, std::string text) {
		neb_set_module_info(nebhandle, modinfo, const_cast<char*>(text.c_str()));
	}

	void Statusengine::RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority) {
		auto result = neb_register_callback(type, nebhandle, priority, callback);

		if (result != 0) {
			ls << "Could not register callback: " << result << eoem;
		}
		else {
			ls << "Register Callback successful" << eom;
		}
	}
}
