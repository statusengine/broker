#include "NagiosProcessData.h"

#include <unistd.h>

#include "NagiosService.h"

namespace statusengine {

	NagiosProcessData::NagiosProcessData(const nebstruct_process_data *processData) {
		SetData<>("type", processData->type);
		SetData<>("flags", processData->flags);
		SetData<>("attr", processData->attr);
		SetData<>("timestamp", processData->timestamp.tv_sec);

		json processdata;
		SetData<>("programmname", std::string("Naemon"));
		SetData<>("modification_data", std::string("removed"));
		SetData<>("programmversion", std::string(get_program_version()));
		SetData<>("pid", getpid());

		SetData<>("processdata", processdata);
	}
}
