#include "Statusengine.h"

#include <sstream>
#include <memory>

#include "vendor/json.hpp"

#include "chacks.h"
#include "LogStream.h"
#include "NagiosHost.h"


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

		RegisterCallback(NEBCALLBACK_HOST_STATUS_DATA, fnptr<int(int, void*)>([this](int event_type, void *data) -> int {
			ls << "callback called :)" << eom;
			auto hostStatus = reinterpret_cast<nebstruct_host_status_data*>(data);
			auto nagHostStatus = reinterpret_cast<host *>(hostStatus->object_ptr);
			auto nagiosHost = std::unique_ptr<NagiosHost>(new NagiosHost(nagHostStatus));
			ls << nagiosHost->GetData().dump() << eom;
			return 0;
		}));
	}

	Statusengine::~Statusengine() {
		ls << "unload" << eom;
		neb_deregister_module_callbacks(nebhandle);
		ls << "unload finished" << eom;
	}

	std::ostream& Statusengine::Log() {
		return ls;
	}

	void Statusengine::SetModuleInfo(int modinfo, std::string text) {
		neb_set_module_info(nebhandle, modinfo, const_cast<char*>(text.c_str()));
	}

	void Statusengine::RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority) {
		auto result = neb_register_callback(type, nebhandle, priority, callback);

		if (result != 0) {
			std::stringstream msg;
			msg << "Could not register callback, error code: ";
			msg << result;
			ls << msg.str() <<eom;
		}
		else {
			ls << "Register Callback successful" << eom;
		}
	}
}
