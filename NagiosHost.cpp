#include "NagiosHost.h"

namespace statusengine {

	NagiosHost::NagiosHost(host * nagiosHostData) : data(nagiosHostData) {
		jsonData = json::object();

		SetData<>("name", data->name);
		SetData<>("current_state", data->current_state);
	}

	std::string NagiosHost::ToString() {
		return jsonData.dump();
	}

	template<typename T>
	void NagiosHost::SetData(std::string name, T value) {
		jsonData[name] = value;
	}

	template<typename T>
	void NagiosHost::SetData(std::string name, T* value) {
		if (value != nullptr) {
			jsonData[name] = value;
		}
	}
}
