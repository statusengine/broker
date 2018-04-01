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
	public:
		Statusengine(nebmodule *handle);
		~Statusengine();

		std::ostream& Log();
		GearmanClient& Gearman();

		template<typename T>
		void RegisterCallback(NebmoduleCallback<T> *cb) {
			RegisterCallback(cb->GetCallbackType(), cb->GetCallbackFunction(), cb->GetPriority());
		};
	private:
		void SetModuleInfo(int modinfo, std::string text);
		void RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority = 0);
		//void DeregisterCallback(NEBCallbackType type, int callback(int, void *));
		
		nebmodule *nebhandle;
		std::ostringstream ls; // logging
		GearmanClient *gearman;
		HostStatusCallback *cb;
	};
}

#endif // !STATUSENGINE_H