#ifndef FLAPPINGDATA_CALLBACK_H
#define FLAPPINGDATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
	class FlappingDataCallback : public NebmoduleCallback<nebstruct_flapping_data> {
	public:
		FlappingDataCallback(Statusengine *se);

		virtual void Callback(int event_type, nebstruct_flapping_data *data);
	};
}

#endif // !FLAPPINGDATA_CALLBACK_H

