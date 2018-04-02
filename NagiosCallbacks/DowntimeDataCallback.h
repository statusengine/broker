#ifndef DOWNTIMEDATA_CALLBACK_H
#define DOWNTIMEDATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
	class DowntimeDataCallback : public NebmoduleCallback<nebstruct_downtime_data> {
	public:
		DowntimeDataCallback(Statusengine *se);

		virtual void Callback(int event_type, nebstruct_downtime_data *data);
	};
}

#endif // !DOWNTIMEDATA_CALLBACK_H

