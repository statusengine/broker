#include "Statusengine.h"

#include <sstream>
#include <memory>

#include "vendor/json.hpp"
#include "libgearman-1.0/gearman.h"

#include "chacks.h"
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

		LogInfo("the missing event broker");
		LogInfo("This is the c++ version of statusengine event broker");

		gearman_client_st *client = gearman_client_create(NULL);

		gearman_return_t ret = gearman_client_add_server(client, "localhost", 0);
		if (gearman_failed(ret)) {
			LogInfo("Gearman Connect Failed");
		}
		else {
			LogInfo("Gearman connect success");
		}
		gearman_client_free(client);

		RegisterCallback(NEBCALLBACK_HOST_STATUS_DATA, fnptr<int(int, void*)>([this](int event_type, void *data) -> int {
			LogInfo("callback called :)");
			auto hostStatus = reinterpret_cast<nebstruct_host_status_data*>(data);
			auto nagHostStatus = reinterpret_cast<host *>(hostStatus->object_ptr);
			auto nagiosHost = std::unique_ptr<NagiosHost>(new NagiosHost(nagHostStatus));
			json j;
			nagiosHost->ToJSON(j);
			LogInfo(j.dump());
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

	void Statusengine::RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority) {
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
