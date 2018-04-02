#include "NagiosLogData.h"


namespace statusengine {

	NagiosLogData::NagiosLogData(const nebstruct_log_data *logData) {
		SetData<>("type", logData->type);
		SetData<>("flags", logData->flags);
		SetData<>("attr", logData->attr);
		SetData<>("timestamp", logData->timestamp.tv_sec);

		json logentry;

		SetData<>("entry_time", logData->entry_time, &logentry);
		SetData<>("data_type", logData->data_type, &logentry);
		SetData<>("data", logData->data, &logentry);

		SetData<>("logentry", logentry);
	}
}
