#ifndef STATUSENGINE_H
#define STATUSENGINE_H

#include <sstream>
#include <string>

#include "nebmodule.h"

#include "GearmanClient.h"
#include "NebmoduleCallback.h"
#include "HostStatusCallback.h"


namespace statusengine {
	class Statusengine {
	private:
		nebmodule *nebhandle;
	public:
		Statusengine(nebmodule *handle);
		~Statusengine();

		std::ostream& Log();
		GearmanClient& Gearman();

		void RegisterCallback(NebmoduleCallback *cb);
	private:
		void SetModuleInfo(int modinfo, std::string text);
		void RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority = 0);
		
		std::ostringstream ls;
		GearmanClient *gearman;
		HostStatusCallback *cb;
	};
}

#endif // !STATUSENGINE_H
