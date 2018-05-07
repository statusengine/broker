#include "NagiosExternalCommandData.h"

namespace statusengine {

    NagiosExternalCommandData::NagiosExternalCommandData(const nebstruct_external_command_data *externalCommandData) {
        SetData<>("type", externalCommandData->type);
        SetData<>("flags", externalCommandData->flags);
        SetData<>("attr", externalCommandData->attr);
        SetData<>("timestamp", externalCommandData->timestamp.tv_sec);

        NagiosObject externalcommand;
        externalcommand.SetData<>("command_string", externalCommandData->command_string);
        externalcommand.SetData<>("command_args", externalCommandData->command_args);
        externalcommand.SetData<>("command_type", externalCommandData->command_type);
        externalcommand.SetData<>("entry_time", externalCommandData->entry_time);

        SetData<>("externalcommand", &externalcommand);
    }
} // namespace statusengine
