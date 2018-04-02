#include "Statusengine.h"

#include <sstream>
#include <memory>

#include "vendor/json.hpp"

#include "LogStream.h"


using json = nlohmann::json;

namespace statusengine {

	Statusengine::Statusengine(nebmodule *handle) : nebhandle(handle) {
		SetModuleInfo(NEBMODULE_MODINFO_TITLE, "Statusengine - the missing event broker");
		SetModuleInfo(NEBMODULE_MODINFO_AUTHOR, "Johannes Drummer");
		SetModuleInfo(NEBMODULE_MODINFO_TITLE, "Copyright (c) 2018 - present Johannes Drummer");
		SetModuleInfo(NEBMODULE_MODINFO_VERSION, "4.0.0");
		SetModuleInfo(NEBMODULE_MODINFO_LICENSE, "GPL v2");
		SetModuleInfo(NEBMODULE_MODINFO_DESC, "A powerful and flexible event broker");

		ls << "the missing event broker" << eom;
		ls << "This is the c++ version of statusengine event broker" << eom;

		gearman = new GearmanClient(ls);

		cbHostStatus = new HostStatusCallback(this);
		cbHostCheck = new HostCheckCallback(this, true, true);
		cbServiceStatus = new ServiceStatusCallback(this);
		cbServiceCheck = new ServiceCheckCallback(this, true, true, true);
		cbStateChange = new StateChangeCallback(this);
		cbLogData = new LogDataCallback(this);
		cbSystemCommandData = new SystemCommandDataCallback(this);
		cbCommentData = new CommentDataCallback(this);
		cbExternalCommandData = new ExternalCommandDataCallback(this);
		cbAcknowledgementData = new AcknowledgementDataCallback(this);
		cbFlappingData = new FlappingDataCallback(this);
		cbDowntimeData = new DowntimeDataCallback(this);
		cbNotificationData = new NotificationDataCallback(this);
		cbProgramStatusData = new ProgramStatusDataCallback(this);
		cbContactStatusData = new ContactStatusDataCallback(this);
		cbContactNotificationData = new ContactNotificationDataCallback(this);
		cbContactNotificationMethodData = new ContactNotificationMethodDataCallback(this);
		cbEventHandlerData = new EventHandlerDataCallback(this);
		cbProcessData = new ProcessDataCallback(this, true, true, true);

		RegisterCallback(cbHostStatus);
		RegisterCallback(cbHostCheck);
		RegisterCallback(cbServiceStatus);
		RegisterCallback(cbServiceCheck);
		RegisterCallback(cbStateChange);
		RegisterCallback(cbLogData);
		RegisterCallback(cbSystemCommandData);
		RegisterCallback(cbCommentData);
		RegisterCallback(cbExternalCommandData);
		RegisterCallback(cbAcknowledgementData);
		RegisterCallback(cbFlappingData);
		RegisterCallback(cbDowntimeData);
		RegisterCallback(cbNotificationData);
		RegisterCallback(cbProgramStatusData);
		RegisterCallback(cbContactStatusData);
		RegisterCallback(cbContactNotificationData);
		RegisterCallback(cbContactNotificationMethodData);
		RegisterCallback(cbEventHandlerData);
		RegisterCallback(cbProcessData);
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

	GearmanClient& Statusengine::Gearman() {
		return *gearman;
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
