#ifndef HOSTCHECK_CALLBACK_H
#define HOSTCHECK_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
	class HostCheckCallback : public NebmoduleCallback<nebstruct_host_check_data> {
	public:
		HostCheckCallback(Statusengine *se, bool hostchecks, bool ocsp);

		virtual void Callback(int event_type, nebstruct_host_check_data *data);

	private:
		bool hostchecks;
		bool ocsp;
	};
}

#endif // !HOSTCHECK_CALLBACK_H
