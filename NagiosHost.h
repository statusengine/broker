#ifndef NAGIOS_HOST_H
#define NAGIOS_HOST_H

#include <string>
#include "vendor/json.hpp"

#include "nebmodule.h"

using json = nlohmann::json;

namespace statusengine {
	class NagiosHost {
	private:
		host *data;
		json jsonData;

	public:
		NagiosHost(host *nagiosHostData);
		std::string ToString();

	private:
		template<typename T>
		void SetData(std::string name, T value);
		template<typename T>
		void SetData(std::string name, T *value);
	};
}

#endif // !NAGIOS_HOST_H
