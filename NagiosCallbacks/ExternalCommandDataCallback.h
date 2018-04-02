#ifndef EXTERNALCOMMANDDATA_CALLBACK_H
#define EXTERNALCOMMANDDATA_CALLBACK_H

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
	class ExternalCommandDataCallback : public NebmoduleCallback<nebstruct_external_command_data> {
	public:
		ExternalCommandDataCallback(Statusengine *se);

		virtual void Callback(int event_type, nebstruct_external_command_data *data);
	};
}

#endif // !EXTERNALCOMMANDDATA_CALLBACK_H

