#ifndef PROCESSDATA_CALLBACK_H
#define PROCESSDATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
	class ProcessDataCallback : public NebmoduleCallback<nebstruct_process_data> {
	public:
		ProcessDataCallback(Statusengine *se, bool restart_data, bool process_data, bool object_data);

		virtual void Callback(int event_type, nebstruct_process_data *data);

	private:
		bool restart_data;
		bool process_data;
		bool object_data;
	};
}

#endif // !PROCESSDATA_CALLBACK_H
