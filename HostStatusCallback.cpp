#include "HostStatusCallback.h"

#include "Statusengine.h"
#include "LogStream.h"

namespace statusengine {
	HostStatusCallback::HostStatusCallback(Statusengine *se) : se(se) {
	}

	void HostStatusCallback::Callback(int event_type, nebstruct_host_status_data *data) {
		se->Log() << "callback called :)" << eom;
	}
}
