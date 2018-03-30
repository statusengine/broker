#include "statusengine.h"

#include <sstream>

#include "chacks.h"
#include "vendor/json.hpp"

using json = nlohmann::json;

namespace statusengine {

	Statusengine::Statusengine(nebmodule *handle) : nebhandle(handle) {
		SetModuleInfo(NEBMODULE_MODINFO_TITLE, "Statusengine - the missing event broker");
		SetModuleInfo(NEBMODULE_MODINFO_AUTHOR, "Johannes Drummer");
		SetModuleInfo(NEBMODULE_MODINFO_TITLE, "Copyright (c) 2018 - present Johannes Drummer");
		SetModuleInfo(NEBMODULE_MODINFO_VERSION, "4.0.0");
		SetModuleInfo(NEBMODULE_MODINFO_LICENSE, "GPL v2");
		SetModuleInfo(NEBMODULE_MODINFO_DESC, "A powerful and flexible event broker");

		LogInfo("the missing event broker");
		LogInfo("This is the c++ version of statusengine event broker");

		RegisterCallback(NEBCALLBACK_HOST_STATUS_DATA, fnptr<int(int, void*)>([this](int event_type, void *data) -> int {
			LogInfo("callback called :)");
			auto hostStatus = reinterpret_cast<nebstruct_host_status_data*>(data);
			auto nagHostStatus = reinterpret_cast<host *>(hostStatus->object_ptr);
			json jso = {
				{"name", nagHostStatus->name}
			};
			LogInfo(jso.dump());
			return 0;
		}));
	}

	Statusengine::~Statusengine() {
		LogInfo("unload");
		neb_deregister_module_callbacks(nebhandle);
		LogInfo("unload finished");
	}

	void Statusengine::SetModuleInfo(int modinfo, std::string text) {
		neb_set_module_info(nebhandle, modinfo, CSTR(text.c_str()));
	}

	void Statusengine::LogInfo(std::string message) {
		nm_log(NSLOG_INFO_MESSAGE, "Statusengine - %s", CSTR(message.c_str()));
	}
//int callback(int, void *)
	void Statusengine::RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority) {
//		auto result = neb_register_callback(type, nebhandle, priority, callback->target<int(int, void*)>());
		auto result = neb_register_callback(type, nebhandle, priority, callback);

		if (result != 0) {
			std::stringstream msg;
			msg << "Could not register callback, error code: ";
			msg << result;
			LogInfo(msg.str());
		}
		else {
			LogInfo("Register Callback successful");
		}
	}
}
