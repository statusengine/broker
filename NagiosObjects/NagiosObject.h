#ifndef NAGIOS_OBJECT_H
#define NAGIOS_OBJECT_H

#include <string>
#include "vendor/json.hpp"

#include "nebmodule.h"

using json = nlohmann::json;

namespace statusengine {
	class NagiosObject {
	public:
		NagiosObject(const json &j);

		json& GetData();

	protected:
		NagiosObject();

		template<typename T>
		void SetData(std::string name, T value) {
			Data[name] = value;
		}

		template<typename T>
		void SetData(std::string name, T *value) {
			if (value != nullptr) {
				if (std::is_same<T, char*>::value) {
					// fancy strings instead of c bullshit
					Data[name] = std::string(reinterpret_cast<char*>(value));
				}
				else {
					Data[name] = value;
				}
			}
		}

		json Data;
	};

}

#endif // !NAGIOS_OBJECT_H
