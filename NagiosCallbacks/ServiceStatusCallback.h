#ifndef SERVICESTATUS_CALLBACK_H
#define SERVICESTATUS_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
	class ServiceStatusCallback : public NebmoduleCallback<nebstruct_service_status_data> {
	public:
		ServiceStatusCallback(Statusengine *se);

		virtual void Callback(int event_type, nebstruct_service_status_data *data);
	};
}

#endif // !SERVICESTATUS_CALLBACK_H