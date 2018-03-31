#ifndef HOSTSTATUS_CALLBACK_H
#define HOSTSTATUS_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
	class HostStatusCallback : public NebmoduleCallback {
	public:
		HostStatusCallback(Statusengine *se);

		virtual void Callback(int event_type, void *data);
	};
}

#endif // !HOSTSTATUS_CALLBACK_H
