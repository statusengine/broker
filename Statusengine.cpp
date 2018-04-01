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
		cbServiceStatus = new ServiceStatusCallback(this);

		RegisterCallback(cbHostStatus);
		RegisterCallback(cbServiceStatus);
	}
	
	Statusengine::~Statusengine() {
		ls << "unload" << eom;
		neb_deregister_module_callbacks(nebhandle);
		ls << "unload finished" << eom;
		delete gearman;

		delete cbHostStatus;
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
