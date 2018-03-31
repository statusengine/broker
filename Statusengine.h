#ifndef STATUSENGINE_H
#define STATUSENGINE_H

#include <sstream>
#include <string>

#include "nebmodule.h"
#include "chacks.h"

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

		template<typename T>
		void RegisterCallback(NebmoduleCallback<T> *cb) {
			RegisterCallback(NEBCALLBACK_HOST_STATUS_DATA, fnptr<int(int, void*)>([cb](int event_type, void *data) -> int {
				cb->RawCallback(event_type, data);
				return 0;
			}));
		};
	private:
		void SetModuleInfo(int modinfo, std::string text);
		void RegisterCallback(NEBCallbackType type, int callback(int, void *), int priority = 0);
		
		std::ostringstream ls;
		GearmanClient *gearman;
		HostStatusCallback *cb;
	};
}

#endif // !STATUSENGINE_H