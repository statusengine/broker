#pragma once

#include <string>

#include <json.h>
#include <unicode/unistr.h>

#include "Nebmodule.h"


namespace statusengine {
    class NagiosObject {
      public:
        explicit NagiosObject() {
            data = json_object_new_object();
        }

        /**
         * This is like a copy constructor, it increments the counter for data
         * @param data
         */
        explicit NagiosObject(json_object *data) : data(json_object_get(data)) {}
        /**
         * This is like a copy constructor, it increments the counter for obj->data
         * @param data
         */
        explicit NagiosObject(statusengine::NagiosObject *obj) : data(json_object_get(obj->data)) {}

        ~NagiosObject() {
            json_object_put(data);
        }

        std::string ToString() {
            return std::string(json_object_to_json_string(data));
        }

        /**
         * Counter will be incremented
         * @return json_object*
         */
        json_object *GetDataCopy() {
            return json_object_get(data);
        }

        inline static std::string EncodeString(char *value) {
            std::string result;
            icu::UnicodeString(value, "UTF-8").toUTF8String(result);
            return result;
        }

        inline void SetData(const char *name, const std::string &value) {
            SetData(data, name, value);
        }

        inline static void SetData(json_object *obj, const char *name, const std::string &value) {
            json_object_object_add(obj, name, json_object_new_string_len(value.c_str(), value.length()));
        }

        inline void SetData(const char *name, const char *value) {
            SetData(data, name, value);
        }

        inline static void SetData(json_object *obj, const char *name, const char *value) {
            if (value == nullptr) {
                json_object_object_add(obj, name, nullptr);
            }
            else {
                json_object_object_add(obj, name, json_object_new_string(value));
            }
        }

        inline void SetData(const char *name, int value) {
            SetData(data, name, value);
        }

        inline static void SetData(json_object *obj, const char *name, int value) {
            json_object_object_add(obj, name, json_object_new_int(value));
        }

        inline void SetData(const char *name, long int value) {
            SetData(data, name, value);
        }

        inline static void SetData(json_object *obj, const char *name, long int value) {
            json_object_object_add(obj, name, json_object_new_int64(value));
        }

        inline void SetData(const char *name, double value) {
            SetData(data, name, value);
        }

        inline static void SetData(json_object *obj, const char *name, double value) {
            json_object_object_add(obj, name, json_object_new_double(value));
        }

        inline void SetData(const char *name, long unsigned int value) {
            SetData(data, name, value);
        }

        inline static void SetData(json_object *obj, const char *name, long unsigned int value) {
            json_object_object_add(obj, name, json_object_new_int64(value));
        }

        inline void SetData(const char *name, NagiosObject *other) {
            SetData(data, name, other);
        }

        inline static void SetData(json_object *obj, const char *name, NagiosObject *other) {
            SetData(obj, name, json_object_get(other->data));
        }

        inline void SetData(const char *name, json_object *other) {
            SetData(data, name, other);
        }

        inline static void SetData(json_object *obj, const char *name, json_object *other) {
            json_object_object_add(obj, name, other);
        }

    protected:
        json_object *data;
    };

    class NagiosProcessData : public NagiosObject {
    public:
        explicit NagiosProcessData(const nebstruct_process_data *processData) {
            SetData("type", processData->type);
            SetData("flags", processData->flags);
            SetData("attr", processData->attr);
            SetData("timestamp", processData->timestamp.tv_sec);

            NagiosObject processdata;
            processdata.SetData("programmname", std::string("Naemon"));
            processdata.SetData("modification_data", std::string("removed"));
            processdata.SetData("programmversion", std::string(get_program_version()));
            SetData("pid", getpid());

            SetData("processdata", &processdata);
        }
    };

    class NagiosAcknowledgementData : public NagiosObject {
    public:
        explicit NagiosAcknowledgementData(const nebstruct_acknowledgement_data *acknowledgementData) {
            SetData("type", acknowledgementData->type);
            SetData("flags", acknowledgementData->flags);
            SetData("attr", acknowledgementData->attr);
            SetData("timestamp", acknowledgementData->timestamp.tv_sec);

            NagiosObject acknowledgement;
            acknowledgement.SetData("host_name", acknowledgementData->host_name);
            acknowledgement.SetData("service_description", acknowledgementData->service_description);
            acknowledgement.SetData("author_name", acknowledgementData->author_name);
            acknowledgement.SetData("comment_data", acknowledgementData->comment_data);
            acknowledgement.SetData("acknowledgement_type", acknowledgementData->acknowledgement_type);
            acknowledgement.SetData("state", acknowledgementData->state);
            acknowledgement.SetData("is_sticky", acknowledgementData->is_sticky);
            acknowledgement.SetData("persistent_comment", acknowledgementData->persistent_comment);
            acknowledgement.SetData("notify_contacts", acknowledgementData->notify_contacts);

            SetData("acknowledgement", &acknowledgement);
        }
    };

    class NagiosCommentData : public NagiosObject {
    public:
        explicit NagiosCommentData(const nebstruct_comment_data *commentData) {
            SetData("type", commentData->type);
            SetData("flags", commentData->flags);
            SetData("attr", commentData->attr);
            SetData("timestamp", commentData->timestamp.tv_sec);

            NagiosObject comment;
            comment.SetData("host_name", commentData->host_name);
            comment.SetData("service_description", commentData->service_description);
            comment.SetData("author_name", commentData->author_name);
            comment.SetData("comment_data", commentData->comment_data);
            comment.SetData("comment_type", commentData->comment_type);
            comment.SetData("entry_time", commentData->entry_time);
            comment.SetData("persistent", commentData->persistent);
            comment.SetData("source", commentData->source);
            comment.SetData("entry_type", commentData->entry_type);
            comment.SetData("expires", commentData->expires);
            comment.SetData("expire_time", commentData->expire_time);
            comment.SetData("comment_id", commentData->comment_id);
            SetData("comment", &comment);
        }
    };

    class NagiosContactNotificationData : public NagiosObject {
    public:
        explicit NagiosContactNotificationData(const nebstruct_contact_notification_data *contactNotificationData) {
            SetData("type", contactNotificationData->type);
            SetData("flags", contactNotificationData->flags);
            SetData("attr", contactNotificationData->attr);
            SetData("timestamp", contactNotificationData->timestamp.tv_sec);

            NagiosObject contactnotificationdata;
            contactnotificationdata.SetData("host_name", contactNotificationData->host_name);
            contactnotificationdata.SetData("service_description", contactNotificationData->service_description);
            contactnotificationdata.SetData("output", EncodeString(contactNotificationData->output));
            contactnotificationdata.SetData("long_output", EncodeString(contactNotificationData->output));
            contactnotificationdata.SetData("ack_author", contactNotificationData->ack_author);
            contactnotificationdata.SetData("ack_data", contactNotificationData->ack_data);
            contactnotificationdata.SetData("contact_name", contactNotificationData->contact_name);
            contactnotificationdata.SetData("state", contactNotificationData->state);
            contactnotificationdata.SetData("reason_type", contactNotificationData->reason_type);
            contactnotificationdata.SetData("end_time", contactNotificationData->end_time.tv_sec);
            contactnotificationdata.SetData("start_time", contactNotificationData->start_time.tv_sec);
            contactnotificationdata.SetData("notification_type", contactNotificationData->notification_type);

            SetData("contactnotificationdata", &contactnotificationdata);
        }
    };

    class NagiosContactNotificationMethodData : public NagiosObject {
    public:
        explicit NagiosContactNotificationMethodData(
                const nebstruct_contact_notification_method_data *contactNotificationMethodData) {
            SetData("type", contactNotificationMethodData->type);
            SetData("flags", contactNotificationMethodData->flags);
            SetData("attr", contactNotificationMethodData->attr);
            SetData("timestamp", contactNotificationMethodData->timestamp.tv_sec);

            NagiosObject contactnotificationmethod;
            contactnotificationmethod.SetData("host_name", contactNotificationMethodData->host_name);
            contactnotificationmethod.SetData("service_description", contactNotificationMethodData->service_description);
            contactnotificationmethod.SetData("output", EncodeString(contactNotificationMethodData->output));
            contactnotificationmethod.SetData("ack_author", contactNotificationMethodData->ack_author);
            contactnotificationmethod.SetData("ack_data", contactNotificationMethodData->ack_data);
            contactnotificationmethod.SetData("contact_name", contactNotificationMethodData->contact_name);
            contactnotificationmethod.SetData("command_name", contactNotificationMethodData->command_name);
            contactnotificationmethod.SetData("command_args", contactNotificationMethodData->command_args);
            contactnotificationmethod.SetData("reason_type", contactNotificationMethodData->reason_type);
            contactnotificationmethod.SetData("state", contactNotificationMethodData->state);
            contactnotificationmethod.SetData("start_time", contactNotificationMethodData->start_time.tv_sec);
            contactnotificationmethod.SetData("end_time", contactNotificationMethodData->end_time.tv_sec);

            SetData("contactnotificationmethod", &contactnotificationmethod);
        }
    };

    class NagiosContactStatusData : public NagiosObject {
    public:
        explicit NagiosContactStatusData(const nebstruct_contact_status_data *contactStatusData) {
            SetData("type", contactStatusData->type);
            SetData("flags", contactStatusData->flags);
            SetData("attr", contactStatusData->attr);
            SetData("timestamp", contactStatusData->timestamp.tv_sec);

            auto tmpContact = reinterpret_cast<contact *>(contactStatusData->object_ptr);

            NagiosObject contactstatus;
            contactstatus.SetData("contact_name", tmpContact->name);
            contactstatus.SetData("host_notifications_enabled", tmpContact->host_notifications_enabled);
            contactstatus.SetData("service_notifications_enabled", tmpContact->service_notifications_enabled);
            contactstatus.SetData("last_host_notification", tmpContact->last_host_notification);
            contactstatus.SetData("last_service_notification", tmpContact->last_service_notification);
            contactstatus.SetData("modified_attributes", tmpContact->modified_attributes);
            contactstatus.SetData("modified_host_attributes", tmpContact->modified_host_attributes);
            contactstatus.SetData("modified_service_attributes", tmpContact->modified_service_attributes);

            SetData("contactstatus", &contactstatus);
        }
    };

    class NagiosDowntimeData : public NagiosObject {
    public:
        explicit NagiosDowntimeData(const nebstruct_downtime_data *downtimeData) {
            SetData("type", downtimeData->type);
            SetData("flags", downtimeData->flags);
            SetData("attr", downtimeData->attr);
            SetData("timestamp", downtimeData->timestamp.tv_sec);

            NagiosObject downtime;
            downtime.SetData("host_name", downtimeData->host_name);
            downtime.SetData("service_description", downtimeData->service_description);
            downtime.SetData("author_name", downtimeData->author_name);
            downtime.SetData("comment_data", downtimeData->comment_data);
            downtime.SetData("host_name", downtimeData->host_name);
            downtime.SetData("downtime_type", downtimeData->downtime_type);
            downtime.SetData("entry_time", downtimeData->entry_time);
            downtime.SetData("start_time", downtimeData->start_time);
            downtime.SetData("end_time", downtimeData->end_time);
            downtime.SetData("triggered_by", downtimeData->triggered_by);
            downtime.SetData("downtime_id", downtimeData->downtime_id);
            downtime.SetData("fixed", downtimeData->fixed);
            downtime.SetData("duration", downtimeData->duration);

            SetData("downtime", &downtime);
        }
    };

    class NagiosEventHandlerData : public NagiosObject {
    public:
        explicit NagiosEventHandlerData(const nebstruct_event_handler_data *eventHandlerData) {
            SetData("type", eventHandlerData->type);
            SetData("flags", eventHandlerData->flags);
            SetData("attr", eventHandlerData->attr);
            SetData("timestamp", eventHandlerData->timestamp.tv_sec);

            NagiosObject eventhandler;
            eventhandler.SetData("host_name", eventHandlerData->host_name);
            eventhandler.SetData("service_description", eventHandlerData->service_description);
            eventhandler.SetData("output", EncodeString(eventHandlerData->output));
            eventhandler.SetData("long_output", EncodeString(eventHandlerData->output));
            eventhandler.SetData("command_name", eventHandlerData->command_name);
            eventhandler.SetData("command_args", eventHandlerData->command_args);
            eventhandler.SetData("command_line", eventHandlerData->command_line);
            eventhandler.SetData("state_type", eventHandlerData->state_type);
            eventhandler.SetData("state", eventHandlerData->state);
            eventhandler.SetData("timeout", eventHandlerData->timeout);
            eventhandler.SetData("early_timeout", eventHandlerData->early_timeout);
            eventhandler.SetData("return_code", eventHandlerData->return_code);
            eventhandler.SetData("execution_time", eventHandlerData->execution_time);
            eventhandler.SetData("start_time", eventHandlerData->start_time.tv_sec);
            eventhandler.SetData("end_time", eventHandlerData->end_time.tv_sec);

            SetData("eventhandler", &eventhandler);
        }
    };

    class NagiosExternalCommandData : public NagiosObject {
    public:
        explicit NagiosExternalCommandData(const nebstruct_external_command_data *externalCommandData) {
            SetData("type", externalCommandData->type);
            SetData("flags", externalCommandData->flags);
            SetData("attr", externalCommandData->attr);
            SetData("timestamp", externalCommandData->timestamp.tv_sec);

            NagiosObject externalcommand;
            externalcommand.SetData("command_string", externalCommandData->command_string);
            externalcommand.SetData("command_args", externalCommandData->command_args);
            externalcommand.SetData("command_type", externalCommandData->command_type);
            externalcommand.SetData("entry_time", externalCommandData->entry_time);

            SetData("externalcommand", &externalcommand);
        }
    };

    class NagiosFlappingData : public NagiosObject {
    public:
        explicit NagiosFlappingData(const nebstruct_flapping_data *flappingData) {
            SetData("type", flappingData->type);
            SetData("flags", flappingData->flags);
            SetData("attr", flappingData->attr);
            SetData("timestamp", flappingData->timestamp.tv_sec);

            comment *tmpComment = nullptr;
            if (flappingData->flapping_type == SERVICE_FLAPPING) {
                tmpComment = find_service_comment(flappingData->comment_id);
            }
            else {
                tmpComment = find_host_comment(flappingData->comment_id);
            }

            NagiosObject flapping;
            flapping.SetData("host_name", flappingData->host_name);
            flapping.SetData("service_description", flappingData->service_description);
            flapping.SetData("flapping_type", flappingData->flapping_type);
            flapping.SetData("comment_id", flappingData->comment_id);
            flapping.SetData("comment_entry_time", (tmpComment == nullptr ? 0 : tmpComment->entry_time));
            flapping.SetData("percent_change", flappingData->percent_change);
            flapping.SetData("high_threshold", flappingData->high_threshold);
            flapping.SetData("low_threshold", flappingData->low_threshold);

            SetData("flapping", &flapping);
        }
    };

    class NagiosHost : public NagiosObject {
    public:
        explicit NagiosHost(const host *data) {
            SetData("name", data->name);
            SetData("plugin_output", EncodeString(data->plugin_output));
            SetData("long_plugin_output", EncodeString(data->long_plugin_output));
            SetData("event_handler", data->event_handler);
            SetData("perf_data", EncodeString(data->perf_data));
            SetData("check_command", data->check_command);
            SetData("check_period", data->check_period);
            SetData("current_state", data->current_state);
            SetData("has_been_checked", data->has_been_checked);
            SetData("should_be_scheduled", 1); // Nagios compatibility
            SetData("current_attempt", data->current_attempt);
            SetData("max_attempts", data->max_attempts);
            SetData("last_check", data->last_check);
            SetData("next_check", data->next_check);
            SetData("check_type", data->check_type);
            SetData("last_state_change", data->last_state_change);
            SetData("last_hard_state_change", data->last_hard_state_change);
            SetData("last_hard_state", data->last_hard_state);
            SetData("last_time_up", data->last_time_up);
            SetData("last_time_down", data->last_time_down);
            SetData("last_time_unreachable", data->last_time_unreachable);
            SetData("state_type", data->state_type);
            SetData("last_notification", data->last_notification);
            SetData("next_notification", data->next_notification);
            SetData("no_more_notifications", data->no_more_notifications);
            SetData("notifications_enabled", data->notifications_enabled);
            SetData("problem_has_been_acknowledged", data->problem_has_been_acknowledged);
            SetData("acknowledgement_type", data->acknowledgement_type);
            SetData("current_notification_number", data->current_notification_number);
            SetData("accept_passive_checks", data->accept_passive_checks);
            SetData("event_handler_enabled", data->event_handler_enabled);
            SetData("checks_enabled", data->checks_enabled);
            SetData("flap_detection_enabled", data->flap_detection_enabled);
            SetData("is_flapping", data->is_flapping);
            SetData("percent_state_change", data->percent_state_change);
            SetData("latency", data->latency);
            SetData("execution_time", data->execution_time);
            SetData("scheduled_downtime_depth", data->scheduled_downtime_depth);
            SetData("process_performance_data", data->process_performance_data);
            SetData("obsess", data->obsess);
            SetData("modified_attributes", data->modified_attributes);
            SetData("check_interval", data->check_interval);
            SetData("retry_interval", data->retry_interval);
        }
    };

    class NagiosHostCheckData : public NagiosObject {
    public:
        explicit NagiosHostCheckData(const nebstruct_host_check_data *hostCheckData) {
            SetData("type", hostCheckData->type);
            SetData("flags", hostCheckData->flags);
            SetData("attr", hostCheckData->attr);
            SetData("timestamp", hostCheckData->timestamp.tv_sec);

            host *nag_host = reinterpret_cast<host *>(hostCheckData->object_ptr);

            char *raw_command = nullptr;
            auto globalMacros = get_global_macros();
            get_raw_command_line_r(globalMacros, nag_host->check_command_ptr, nag_host->check_command, &raw_command, 0);

            NagiosObject hostcheck;

            hostcheck.SetData("host_name", hostCheckData->host_name);
            hostcheck.SetData("command_line", raw_command != nullptr ? raw_command : nullptr);
            hostcheck.SetData("command_name", nag_host->check_command != nullptr ? nag_host->check_command : nullptr);
            hostcheck.SetData("output", EncodeString(hostCheckData->output));
            hostcheck.SetData("long_output", EncodeString(hostCheckData->long_output));
            hostcheck.SetData("perf_data", EncodeString(hostCheckData->perf_data));
            hostcheck.SetData("check_type", hostCheckData->check_type);
            hostcheck.SetData("current_attempt", hostCheckData->current_attempt);
            hostcheck.SetData("max_attempts", hostCheckData->max_attempts);
            hostcheck.SetData("state_type", hostCheckData->state_type);
            hostcheck.SetData("state", hostCheckData->state);
            hostcheck.SetData("timeout", hostCheckData->timeout);
            hostcheck.SetData("start_time", hostCheckData->start_time.tv_sec);
            hostcheck.SetData("end_time", hostCheckData->end_time.tv_sec);
            hostcheck.SetData("early_timeout", hostCheckData->early_timeout);
            hostcheck.SetData("execution_time", hostCheckData->execution_time);
            hostcheck.SetData("latency", hostCheckData->latency);
            hostcheck.SetData("return_code", hostCheckData->return_code);

            SetData("hostcheck", &hostcheck);

            clear_volatile_macros_r(globalMacros);
        }
    };

    class NagiosHostStatusData : public NagiosObject {
    public:
        explicit NagiosHostStatusData(const nebstruct_host_status_data *hostStatusData) {
            SetData("type", hostStatusData->type);
            SetData("flags", hostStatusData->flags);
            SetData("attr", hostStatusData->attr);
            SetData("timestamp", hostStatusData->timestamp.tv_sec);
            NagiosHost hostStatus(reinterpret_cast<host *>(hostStatusData->object_ptr));
            SetData("hoststatus", &hostStatus);
        }
    };

    class NagiosLogData : public NagiosObject {
    public:
        explicit NagiosLogData(const nebstruct_log_data *logData) {
            SetData("type", logData->type);
            SetData("flags", logData->flags);
            SetData("attr", logData->attr);
            SetData("timestamp", logData->timestamp.tv_sec);

            NagiosObject logentry;

            logentry.SetData("entry_time", logData->entry_time);
            logentry.SetData("data_type", logData->data_type);
            logentry.SetData("data", logData->data);

            SetData("logentry", &logentry);
        }
    };

    class NagiosNotificationData : public NagiosObject {
    public:
        explicit NagiosNotificationData(const nebstruct_notification_data *notificationData) {
            SetData("type", notificationData->type);
            SetData("flags", notificationData->flags);
            SetData("attr", notificationData->attr);
            SetData("timestamp", notificationData->timestamp.tv_sec);

            NagiosObject notification_data;

            notification_data.SetData("host_name", notificationData->host_name);
            notification_data.SetData("service_description", notificationData->service_description);
            notification_data.SetData("output", EncodeString(notificationData->output));
            notification_data.SetData("long_output", EncodeString(notificationData->output));
            notification_data.SetData("ack_author", notificationData->ack_author);
            notification_data.SetData("ack_data", notificationData->ack_data);
            notification_data.SetData("notification_type", notificationData->notification_type);
            notification_data.SetData("start_time", notificationData->start_time.tv_sec);
            notification_data.SetData("end_time", notificationData->end_time.tv_sec);
            notification_data.SetData("reason_type", notificationData->reason_type);
            notification_data.SetData("state", notificationData->state);
            notification_data.SetData("escalated", notificationData->escalated);
            notification_data.SetData("contacts_notified", notificationData->contacts_notified);

            SetData("notification_data", &notification_data);
        }
    };

    class NagiosProgramStatusData : public NagiosObject {
    public:
        explicit NagiosProgramStatusData(const nebstruct_program_status_data *programmStatusData) {
            SetData("type", programmStatusData->type);
            SetData("flags", programmStatusData->flags);
            SetData("attr", programmStatusData->attr);
            SetData("timestamp", programmStatusData->timestamp.tv_sec);

            NagiosObject programmstatus;
            programmstatus.SetData("global_host_event_handler", programmStatusData->global_host_event_handler);
            programmstatus.SetData("global_service_event_handler", programmStatusData->global_service_event_handler);
            programmstatus.SetData("program_start", programmStatusData->program_start);
            programmstatus.SetData("pid", programmStatusData->pid);
            programmstatus.SetData("daemon_mode", programmStatusData->daemon_mode);
            programmstatus.SetData("last_command_check", 0);
            programmstatus.SetData("last_log_rotation", programmStatusData->last_log_rotation);
            programmstatus.SetData("notifications_enabled", programmStatusData->notifications_enabled);
            programmstatus.SetData("active_service_checks_enabled", programmStatusData->active_service_checks_enabled);
            programmstatus.SetData("passive_service_checks_enabled", programmStatusData->passive_service_checks_enabled);
            programmstatus.SetData("active_host_checks_enabled", programmStatusData->active_host_checks_enabled);
            programmstatus.SetData("passive_host_checks_enabled", programmStatusData->passive_host_checks_enabled);
            programmstatus.SetData("event_handlers_enabled", programmStatusData->event_handlers_enabled);
            programmstatus.SetData("flap_detection_enabled", programmStatusData->flap_detection_enabled);
            programmstatus.SetData("failure_prediction_enabled", 0);
            programmstatus.SetData("process_performance_data", programmStatusData->process_performance_data);
            programmstatus.SetData("obsess_over_hosts", programmStatusData->obsess_over_hosts);
            programmstatus.SetData("obsess_over_services", programmStatusData->obsess_over_services);
            programmstatus.SetData("modified_host_attributes", programmStatusData->modified_host_attributes);
            programmstatus.SetData("modified_service_attributes", programmStatusData->modified_service_attributes);

            SetData("programmstatus", &programmstatus);
        }
    };

    class NagiosService : public NagiosObject {
    public:
        explicit NagiosService(const service *data) {
            SetData("host_name", data->host_name);
            SetData("description", data->description);
            SetData("plugin_output", EncodeString(data->plugin_output));
            SetData("long_plugin_output", EncodeString(data->long_plugin_output));
            SetData("event_handler", data->event_handler);
            SetData("perf_data", EncodeString(data->perf_data));
            SetData("check_command", data->check_command);
            SetData("check_period", data->check_period);
            SetData("current_state", data->current_state);
            SetData("has_been_checked", data->has_been_checked);
            SetData("should_be_scheduled", 1); // Nagios Compatibility
            SetData("current_attempt", data->current_attempt);
            SetData("max_attempts", data->max_attempts);
            SetData("last_check", data->last_check);
            SetData("next_check", data->next_check);
            SetData("check_type", data->check_type);
            SetData("last_state_change", data->last_state_change);
            SetData("last_hard_state_change", data->last_hard_state_change);
            SetData("last_hard_state", data->last_hard_state);
            SetData("last_time_ok", data->last_time_ok);
            SetData("last_time_warning", data->last_time_warning);
            SetData("last_time_critical", data->last_time_critical);
            SetData("last_time_unknown", data->last_time_unknown);
            SetData("state_type", data->state_type);
            SetData("last_notification", data->last_notification);
            SetData("next_notification", data->next_notification);
            SetData("no_more_notifications", data->no_more_notifications);
            SetData("notifications_enabled", data->notifications_enabled);
            SetData("problem_has_been_acknowledged", data->problem_has_been_acknowledged);
            SetData("acknowledgement_type", data->acknowledgement_type);
            SetData("current_notification_number", data->current_notification_number);
            SetData("accept_passive_checks", data->accept_passive_checks);
            SetData("event_handler_enabled", data->event_handler_enabled);
            SetData("checks_enabled", data->checks_enabled);
            SetData("flap_detection_enabled", data->flap_detection_enabled);
            SetData("is_flapping", data->is_flapping);
            SetData("percent_state_change", data->percent_state_change);
            SetData("latency", data->latency);
            SetData("execution_time", data->execution_time);
            SetData("scheduled_downtime_depth", data->scheduled_downtime_depth);
            SetData("process_performance_data", data->process_performance_data);
            SetData("obsess", data->obsess);
            SetData("modified_attributes", data->modified_attributes);
            SetData("check_interval", data->check_interval);
            SetData("retry_interval", data->retry_interval);
        }
    };

    class NagiosServiceCheckData : public NagiosObject {
    public:
        explicit NagiosServiceCheckData(const nebstruct_service_check_data *serviceCheckData) {
            SetData("type", serviceCheckData->type);
            SetData("flags", serviceCheckData->flags);
            SetData("attr", serviceCheckData->attr);
            SetData("timestamp", serviceCheckData->timestamp.tv_sec);

            service *nag_service = reinterpret_cast<service *>(serviceCheckData->object_ptr);

            char *raw_command = nullptr;
            get_raw_command_line_r(get_global_macros(), nag_service->check_command_ptr, nag_service->check_command,
                                   &raw_command, 0);

            NagiosObject servicecheck;
            servicecheck.SetData("host_name", serviceCheckData->host_name);
            servicecheck.SetData("service_description", serviceCheckData->service_description);
            servicecheck.SetData("command_line", (raw_command != nullptr ? raw_command : nullptr));
            servicecheck.SetData("command_name",
                                   (nag_service->check_command != nullptr ? nag_service->check_command : nullptr));
            servicecheck.SetData("output", EncodeString(serviceCheckData->output));
            servicecheck.SetData("long_output", EncodeString(serviceCheckData->long_output));
            servicecheck.SetData("perf_data", EncodeString(serviceCheckData->perf_data));
            servicecheck.SetData("check_type", serviceCheckData->check_type);
            servicecheck.SetData("current_attempt", serviceCheckData->current_attempt);
            servicecheck.SetData("max_attempts", serviceCheckData->max_attempts);
            servicecheck.SetData("state_type", serviceCheckData->state_type);
            servicecheck.SetData("state", serviceCheckData->state);
            servicecheck.SetData("timeout", serviceCheckData->timeout);
            servicecheck.SetData("start_time", serviceCheckData->start_time.tv_sec);
            servicecheck.SetData("end_time", serviceCheckData->end_time.tv_sec);
            servicecheck.SetData("early_timeout", serviceCheckData->early_timeout);
            servicecheck.SetData("execution_time", serviceCheckData->execution_time);
            servicecheck.SetData("latency", serviceCheckData->latency);
            servicecheck.SetData("return_code", serviceCheckData->return_code);
            SetData("servicecheck", &servicecheck);
        }
    };

    class NagiosServiceCheckPerfData : public NagiosObject {
    public:
        explicit NagiosServiceCheckPerfData(const nebstruct_service_check_data *serviceCheckData) {
            SetData("type", serviceCheckData->type);
            SetData("flags", serviceCheckData->flags);
            SetData("attr", serviceCheckData->attr);
            SetData("timestamp", serviceCheckData->timestamp.tv_sec);

            NagiosObject servicecheck;

            servicecheck.SetData("host_name", serviceCheckData->host_name);
            servicecheck.SetData("service_description", serviceCheckData->service_description);
            servicecheck.SetData("perf_data", EncodeString(serviceCheckData->perf_data));
            servicecheck.SetData("start_time", serviceCheckData->start_time.tv_sec);
            SetData("servicecheck", &servicecheck);
        }
    };

    class NagiosServiceStatusData : public NagiosObject {
    public:
        explicit NagiosServiceStatusData(const nebstruct_service_status_data *serviceStatusData) {
            SetData("type", serviceStatusData->type);
            SetData("flags", serviceStatusData->flags);
            SetData("attr", serviceStatusData->attr);
            SetData("timestamp", serviceStatusData->timestamp.tv_sec);
            NagiosService serviceStatus(reinterpret_cast<service *>(serviceStatusData->object_ptr));
            SetData("servicestatus", &serviceStatus);
        }
    };

    class NagiosStateChangeData : public NagiosObject {
    public:
        explicit NagiosStateChangeData(const nebstruct_statechange_data *stateChangeData) {
            SetData("type", stateChangeData->type);
            SetData("flags", stateChangeData->flags);
            SetData("attr", stateChangeData->attr);
            SetData("timestamp", stateChangeData->timestamp.tv_sec);

            int last_state;
            int last_hard_state;

            if (stateChangeData->statechange_type == SERVICE_STATECHANGE) {
                auto tmp_service = reinterpret_cast<service *>(stateChangeData->object_ptr);
                last_state = tmp_service->last_state;
                last_hard_state = tmp_service->last_hard_state;
            }
            else {
                auto tmp_host = reinterpret_cast<host *>(stateChangeData->object_ptr);
                last_state = tmp_host->last_state;
                last_hard_state = tmp_host->last_hard_state;
            }

            NagiosObject statechange;

            statechange.SetData("host_name", stateChangeData->host_name);
            statechange.SetData("service_description", stateChangeData->service_description);
            statechange.SetData("output", EncodeString(stateChangeData->output));
            statechange.SetData("long_output", EncodeString(stateChangeData->output));
            statechange.SetData("statechange_type", stateChangeData->statechange_type);
            statechange.SetData("state", stateChangeData->state);
            statechange.SetData("state_type", stateChangeData->state_type);
            statechange.SetData("current_attempt", stateChangeData->current_attempt);
            statechange.SetData("max_attempts", stateChangeData->max_attempts);
            statechange.SetData("last_state", last_state);
            statechange.SetData("last_hard_state", last_hard_state);

            SetData("statechange", &statechange);
        }
    };

    class NagiosSystemCommandData : public NagiosObject {
    public:
        explicit NagiosSystemCommandData(const nebstruct_system_command_data *systemCommandData) {
            SetData("type", systemCommandData->type);
            SetData("flags", systemCommandData->flags);
            SetData("attr", systemCommandData->attr);
            SetData("timestamp", systemCommandData->timestamp.tv_sec);

            NagiosObject systemcommand;

            systemcommand.SetData("command_line", systemCommandData->command_line);
            systemcommand.SetData("output", EncodeString(systemCommandData->output));
            systemcommand.SetData("long_output", EncodeString(systemCommandData->output));
            systemcommand.SetData("start_time", systemCommandData->start_time.tv_sec);
            systemcommand.SetData("end_time", systemCommandData->end_time.tv_sec);
            systemcommand.SetData("timeout", systemCommandData->timeout);
            systemcommand.SetData("early_timeout", systemCommandData->early_timeout);
            systemcommand.SetData("return_code", systemCommandData->return_code);
            systemcommand.SetData("execution_time", systemCommandData->execution_time);

            SetData("systemcommand", &systemcommand);
        }
    };
} // namespace statusengine
