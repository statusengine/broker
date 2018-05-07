#include "NagiosProgramStatusData.h"

namespace statusengine {

    NagiosProgramStatusData::NagiosProgramStatusData(const nebstruct_program_status_data *programmStatusData) {
        SetData<>("type", programmStatusData->type);
        SetData<>("flags", programmStatusData->flags);
        SetData<>("attr", programmStatusData->attr);
        SetData<>("timestamp", programmStatusData->timestamp.tv_sec);

        NagiosObject programmstatus;
        programmstatus.SetData<>("global_host_event_handler", programmStatusData->global_host_event_handler);
        programmstatus.SetData<>("global_service_event_handler", programmStatusData->global_service_event_handler);
        programmstatus.SetData<>("program_start", programmStatusData->program_start);
        programmstatus.SetData<>("pid", programmStatusData->pid);
        programmstatus.SetData<>("daemon_mode", programmStatusData->daemon_mode);
        programmstatus.SetData<>("last_command_check", 0);
        programmstatus.SetData<>("last_log_rotation", programmStatusData->last_log_rotation);
        programmstatus.SetData<>("notifications_enabled", programmStatusData->notifications_enabled);
        programmstatus.SetData<>("active_service_checks_enabled", programmStatusData->active_service_checks_enabled);
        programmstatus.SetData<>("passive_service_checks_enabled", programmStatusData->passive_service_checks_enabled);
        programmstatus.SetData<>("active_host_checks_enabled", programmStatusData->active_host_checks_enabled);
        programmstatus.SetData<>("passive_host_checks_enabled", programmStatusData->passive_host_checks_enabled);
        programmstatus.SetData<>("event_handlers_enabled", programmStatusData->event_handlers_enabled);
        programmstatus.SetData<>("flap_detection_enabled", programmStatusData->flap_detection_enabled);
        programmstatus.SetData<>("failure_prediction_enabled", 0);
        programmstatus.SetData<>("process_performance_data", programmStatusData->process_performance_data);
        programmstatus.SetData<>("obsess_over_hosts", programmStatusData->obsess_over_hosts);
        programmstatus.SetData<>("obsess_over_services", programmStatusData->obsess_over_services);
        programmstatus.SetData<>("modified_host_attributes", programmStatusData->modified_host_attributes);
        programmstatus.SetData<>("modified_service_attributes", programmStatusData->modified_service_attributes);

        SetData<>("programmstatus", &programmstatus);
    }
} // namespace statusengine
