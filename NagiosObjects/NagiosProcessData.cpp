#include "NagiosProcessData.h"

#include <unistd.h>

#include "NagiosService.h"

namespace statusengine {

    NagiosProcessData::NagiosProcessData(
        const nebstruct_process_data *processData) {
        SetData<>("type", processData->type);
        SetData<>("flags", processData->flags);
        SetData<>("attr", processData->attr);
        SetData<>("timestamp", processData->timestamp.tv_sec);

        json_object *processdata = json_object_new_object();
        SetData<>("programmname", std::string("Naemon"), processdata);
        SetData<>("modification_data", std::string("removed"), processdata);
        SetData<>("programmversion", std::string(get_program_version()),
                  processdata);
        SetData<>("pid", getpid());

        SetData<>("processdata", processdata);
    }
} // namespace statusengine
