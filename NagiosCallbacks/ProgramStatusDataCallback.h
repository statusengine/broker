#ifndef PROGRAMSTATUSDATA_CALLBACK_H
#define PROGRAMSTATUSDATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
	class ProgramStatusDataCallback : public NebmoduleCallback<nebstruct_program_status_data> {
	public:
		ProgramStatusDataCallback(Statusengine *se);

		virtual void Callback(int event_type, nebstruct_program_status_data *data);
	};
}

#endif // !PROGRAMSTATUSDATA_CALLBACK_H

