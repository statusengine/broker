#include "HostStatusCallback.h"

#include "Statusengine.h"
#include "LogStream.h"

namespace statusengine {
	HostStatusCallback::HostStatusCallback(Statusengine *se) : NebmoduleCallback(se) {
	}

	void HostStatusCallback::Callback(int event_type, void *data) {
		se->Log() << "callback called :)" << eom;
	}
}
