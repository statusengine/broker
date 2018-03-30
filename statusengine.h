#ifndef STATUSENGINE_H
#define STATUSENGINE_H

#include <functional>
#include <string>

#include "nebmodule.h"

namespace statusengine {
	class Statusengine {
	private:
		nebmodule *nebhandle;
	public:
		Statusengine(nebmodule *handle);
		~Statusengine();

		void LogInfo(std::string message);
	private:
		void SetModuleInfo(int modinfo, std::string text);
		void RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority = 0);

	};
}

#endif // !STATUSENGINE_H
