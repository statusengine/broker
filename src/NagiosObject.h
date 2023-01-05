#pragma once

#include <string>

#include <json.h>

#include "Nebmodule.h"


namespace statusengine {
    class NagiosObject {
      public:
        explicit NagiosObject(INebmodule &neb) : nebmodule(neb) {
            data = json_object_new_object();
        }

        /**
         * This is like a copy constructor, it increments the counter for data
         * @param data
        
        explicit NagiosObject(json_object *data) : nebmodule(Nebmodule::Instance()), data(json_object_get(data)) {}
         */
        explicit NagiosObject(json_object *data) = delete;

        /**
         * This is like a copy constructor, it increments the counter for obj->data
         * @param data
         */
        explicit NagiosObject(statusengine::NagiosObject *obj) : nebmodule(obj->nebmodule), data(json_object_get(obj->data)) {}

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
        INebmodule &nebmodule;
        json_object *data;
    };

    class NagiosProcessData : public NagiosObject {
    public:
        explicit NagiosProcessData(INebmodule &neb, const nebstruct_process_data *processData) : NagiosObject(neb) {
            SetData("type", processData->type);
            SetData("flags", processData->flags);
            SetData("attr", processData->attr);
            SetData("timestamp", processData->timestamp.tv_sec);
            SetData("timestamp_usec", processData->timestamp.tv_usec);

            NagiosObject processdata(neb);
            processdata.SetData("programmname", std::string("Naemon"));
            processdata.SetData("modification_data", std::string("removed"));
            processdata.SetData("programmversion", std::string(get_program_version()));
            SetData("pid", getpid());

            SetData("processdata", &processdata);
        }
    };

    class NagiosAcknowledgementData : public NagiosObject {
    public:
        explicit NagiosAcknowledgementData(INebmodule &neb, const nebstruct_acknowledgement_data *acknowledgementData) : NagiosObject(neb) {
            SetData("type", acknowledgementData->type);
            SetData("flags", acknowledgementData->flags);
            SetData("attr", acknowledgementData->attr);
            SetData("timestamp", acknowledgementData->timestamp.tv_sec);
            SetData("timestamp_usec", acknowledgementData->timestamp.tv_usec);

            NagiosObject acknowledgement(neb);
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
        explicit NagiosCommentData(INebmodule &neb, const nebstruct_comment_data *commentData) : NagiosObject(neb) {
            SetData("type", commentData->type);
            SetData("flags", commentData->flags);
            SetData("attr", commentData->attr);
            SetData("timestamp", commentData->timestamp.tv_sec);
            SetData("timestamp_usec", commentData->timestamp.tv_usec);

            NagiosObject comment(neb);
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
        explicit NagiosContactNotificationData(INebmodule &neb, const nebstruct_contact_notification_data *contactNotificationData) : NagiosObject(neb) {
            SetData("type", contactNotificationData->type);
            SetData("flags", contactNotificationData->flags);
            SetData("attr", contactNotificationData->attr);
            SetData("timestamp", contactNotificationData->timestamp.tv_sec);
            SetData("timestamp_usec", contactNotificationData->timestamp.tv_usec);

            NagiosObject contactnotificationdata(neb);
            contactnotificationdata.SetData("host_name", contactNotificationData->host_name);
            contactnotificationdata.SetData("service_description", contactNotificationData->service_description);
            contactnotificationdata.SetData("output", nebmodule.EncodeString(contactNotificationData->output));
            contactnotificationdata.SetData("long_output", nebmodule.EncodeString(contactNotificationData->output));
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
        explicit NagiosContactNotificationMethodData(INebmodule &neb, 
                const nebstruct_contact_notification_method_data *contactNotificationMethodData) : NagiosObject(neb) {
            SetData("type", contactNotificationMethodData->type);
            SetData("flags", contactNotificationMethodData->flags);
            SetData("attr", contactNotificationMethodData->attr);
            SetData("timestamp", contactNotificationMethodData->timestamp.tv_sec);
            SetData("timestamp_usec", contactNotificationMethodData->timestamp.tv_usec);

            NagiosObject contactnotificationmethod(neb);
            contactnotificationmethod.SetData("host_name", contactNotificationMethodData->host_name);
            contactnotificationmethod.SetData("service_description", contactNotificationMethodData->service_description);
            contactnotificationmethod.SetData("output", nebmodule.EncodeString(contactNotificationMethodData->output));
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
        explicit NagiosContactStatusData(INebmodule &neb, const nebstruct_contact_status_data *contactStatusData) : NagiosObject(neb) {
            SetData("type", contactStatusData->type);
            SetData("flags", contactStatusData->flags);
            SetData("attr", contactStatusData->attr);
            SetData("timestamp", contactStatusData->timestamp.tv_sec);
            SetData("timestamp_usec", contactStatusData->timestamp.tv_usec);

            auto tmpContact = reinterpret_cast<contact *>(contactStatusData->object_ptr);

            NagiosObject contactstatus(neb);
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
        explicit NagiosDowntimeData(INebmodule &neb, const nebstruct_downtime_data *downtimeData) : NagiosObject(neb) {
            SetData("type", downtimeData->type);
            SetData("flags", downtimeData->flags);
            SetData("attr", downtimeData->attr);
            SetData("timestamp", downtimeData->timestamp.tv_sec);
            SetData("timestamp_usec", downtimeData->timestamp.tv_usec);

            NagiosObject downtime(neb);
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
        explicit NagiosEventHandlerData(INebmodule &neb, const nebstruct_event_handler_data *eventHandlerData) : NagiosObject(neb) {
            SetData("type", eventHandlerData->type);
            SetData("flags", eventHandlerData->flags);
            SetData("attr", eventHandlerData->attr);
            SetData("timestamp", eventHandlerData->timestamp.tv_sec);
            SetData("timestamp_usec", eventHandlerData->timestamp.tv_usec);

            NagiosObject eventhandler(neb);
            eventhandler.SetData("host_name", eventHandlerData->host_name);
            eventhandler.SetData("service_description", eventHandlerData->service_description);
            eventhandler.SetData("output", nebmodule.EncodeString(eventHandlerData->output));
            eventhandler.SetData("long_output", nebmodule.EncodeString(eventHandlerData->output));
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
        explicit NagiosExternalCommandData(INebmodule &neb, const nebstruct_external_command_data *externalCommandData) : NagiosObject(neb) {
            SetData("type", externalCommandData->type);
            SetData("flags", externalCommandData->flags);
            SetData("attr", externalCommandData->attr);
            SetData("timestamp", externalCommandData->timestamp.tv_sec);
            SetData("timestamp_usec", externalCommandData->timestamp.tv_usec);

            NagiosObject externalcommand(neb);
            externalcommand.SetData("command_string", externalCommandData->command_string);
            externalcommand.SetData("command_args", externalCommandData->command_args);
            externalcommand.SetData("command_type", externalCommandData->command_type);
            externalcommand.SetData("entry_time", externalCommandData->entry_time);

            SetData("externalcommand", &externalcommand);
        }
    };

    class NagiosFlappingData : public NagiosObject {
    public:
        explicit NagiosFlappingData(INebmodule &neb, const nebstruct_flapping_data *flappingData) : NagiosObject(neb) {
            SetData("type", flappingData->type);
            SetData("flags", flappingData->flags);
            SetData("attr", flappingData->attr);
            SetData("timestamp", flappingData->timestamp.tv_sec);
            SetData("timestamp_usec", flappingData->timestamp.tv_usec);

            comment *tmpComment = nullptr;
            if (flappingData->flapping_type == SERVICE_FLAPPING) {
                tmpComment = find_service_comment(flappingData->comment_id);
            }
            else {
                tmpComment = find_host_comment(flappingData->comment_id);
            }

            NagiosObject flapping(neb);
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
        explicit NagiosHost(INebmodule &neb, const host *data) : NagiosObject(neb) {
            SetData("name", data->name);
            SetData("plugin_output", nebmodule.EncodeString(data->plugin_output));
            SetData("long_plugin_output", nebmodule.EncodeString(data->long_plugin_output));
            SetData("event_handler", data->event_handler);
            SetData("perf_data", nebmodule.EncodeString(data->perf_data));
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
        explicit NagiosHostCheckData(INebmodule &neb, const nebstruct_host_check_data *hostCheckData) : NagiosObject(neb) {
            SetData("type", hostCheckData->type);
            SetData("flags", hostCheckData->flags);
            SetData("attr", hostCheckData->attr);
            SetData("timestamp", hostCheckData->timestamp.tv_sec);
            SetData("timestamp_usec", hostCheckData->timestamp.tv_usec);

            host *nag_host = reinterpret_cast<host *>(hostCheckData->object_ptr);

            auto globalMacros = get_global_macros();
            get_raw_command_line_r(globalMacros, nag_host->check_command_ptr, nag_host->check_command, &raw_command, 0);

            NagiosObject hostcheck(neb);

            hostcheck.SetData("host_name", hostCheckData->host_name);
            hostcheck.SetData("command_line", raw_command != nullptr ? raw_command : nullptr);
            hostcheck.SetData("command_name", nag_host->check_command != nullptr ? nag_host->check_command : nullptr);
            hostcheck.SetData("output", nebmodule.EncodeString(hostCheckData->output));
            hostcheck.SetData("long_output", nebmodule.EncodeString(hostCheckData->long_output));
            hostcheck.SetData("perf_data", nebmodule.EncodeString(hostCheckData->perf_data));
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

        ~NagiosHostCheckData() {
            free(raw_command);
        }
    private:
        char *raw_command = nullptr;
    };

    class NagiosHostStatusData : public NagiosObject {
    public:
        explicit NagiosHostStatusData(INebmodule &neb, const nebstruct_host_status_data *hostStatusData) : NagiosObject(neb) {
            SetData("type", hostStatusData->type);
            SetData("flags", hostStatusData->flags);
            SetData("attr", hostStatusData->attr);
            SetData("timestamp", hostStatusData->timestamp.tv_sec);
            SetData("timestamp_usec", hostStatusData->timestamp.tv_usec);

            NagiosHost hostStatus(neb, reinterpret_cast<host *>(hostStatusData->object_ptr));
            SetData("hoststatus", &hostStatus);
        }
    };

    class NagiosLogData : public NagiosObject {
    public:
        explicit NagiosLogData(INebmodule &neb, const nebstruct_log_data *logData) : NagiosObject(neb) {
            SetData("type", logData->type);
            SetData("flags", logData->flags);
            SetData("attr", logData->attr);
            SetData("timestamp", logData->timestamp.tv_sec);
            SetData("timestamp_usec", logData->timestamp.tv_usec);

            NagiosObject logentry(neb);

            logentry.SetData("entry_time", logData->entry_time);
            logentry.SetData("data_type", logData->data_type);
            logentry.SetData("data", logData->data);

            SetData("logentry", &logentry);
        }
    };

    class NagiosNotificationData : public NagiosObject {
    public:
        explicit NagiosNotificationData(INebmodule &neb, const nebstruct_notification_data *notificationData) : NagiosObject(neb) {
            SetData("type", notificationData->type);
            SetData("flags", notificationData->flags);
            SetData("attr", notificationData->attr);
            SetData("timestamp", notificationData->timestamp.tv_sec);
            SetData("timestamp_usec", notificationData->timestamp.tv_usec);

            NagiosObject notification_data(neb);

            notification_data.SetData("host_name", notificationData->host_name);
            notification_data.SetData("service_description", notificationData->service_description);
            notification_data.SetData("output", nebmodule.EncodeString(notificationData->output));
            notification_data.SetData("long_output", nebmodule.EncodeString(notificationData->output));
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
        explicit NagiosProgramStatusData(INebmodule &neb, const nebstruct_program_status_data *programmStatusData) : NagiosObject(neb) {
            SetData("type", programmStatusData->type);
            SetData("flags", programmStatusData->flags);
            SetData("attr", programmStatusData->attr);
            SetData("timestamp", programmStatusData->timestamp.tv_sec);
            SetData("timestamp_usec", programmStatusData->timestamp.tv_usec);

            NagiosObject programmstatus(neb);
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
        explicit NagiosService(INebmodule &neb, const service *data) : NagiosObject(neb) {
            SetData("host_name", data->host_name);
            SetData("description", data->description);
            SetData("plugin_output", nebmodule.EncodeString(data->plugin_output));
            SetData("long_plugin_output", nebmodule.EncodeString(data->long_plugin_output));
            SetData("event_handler", data->event_handler);
            SetData("perf_data", nebmodule.EncodeString(data->perf_data));
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
        explicit NagiosServiceCheckData(INebmodule &neb, const nebstruct_service_check_data *serviceCheckData) : NagiosObject(neb) {
            SetData("type", serviceCheckData->type);
            SetData("flags", serviceCheckData->flags);
            SetData("attr", serviceCheckData->attr);
            SetData("timestamp", serviceCheckData->timestamp.tv_sec);
            SetData("timestamp_usec", serviceCheckData->timestamp.tv_usec);

            service *nag_service = reinterpret_cast<service *>(serviceCheckData->object_ptr);

            auto globalMacros = get_global_macros();
            get_raw_command_line_r(globalMacros, nag_service->check_command_ptr, nag_service->check_command,
                                   &raw_command, 0);

            NagiosObject servicecheck(neb);
            servicecheck.SetData("host_name", serviceCheckData->host_name);
            servicecheck.SetData("service_description", serviceCheckData->service_description);
            servicecheck.SetData("command_line", (raw_command != nullptr ? raw_command : nullptr));
            servicecheck.SetData("command_name",
                                   (nag_service->check_command != nullptr ? nag_service->check_command : nullptr));
            servicecheck.SetData("output", nebmodule.EncodeString(serviceCheckData->output));
            servicecheck.SetData("long_output", nebmodule.EncodeString(serviceCheckData->long_output));
            servicecheck.SetData("perf_data", nebmodule.EncodeString(serviceCheckData->perf_data));
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

            clear_volatile_macros_r(globalMacros);
        }

        ~NagiosServiceCheckData() {
            free(raw_command);
        }

    private:
        char *raw_command = nullptr;
    };

    class NagiosServiceCheckPerfData : public NagiosObject {
    public:
        explicit NagiosServiceCheckPerfData(INebmodule &neb, const nebstruct_service_check_data *serviceCheckData) : NagiosObject(neb) {
            SetData("type", serviceCheckData->type);
            SetData("flags", serviceCheckData->flags);
            SetData("attr", serviceCheckData->attr);
            SetData("timestamp", serviceCheckData->timestamp.tv_sec);
            SetData("timestamp_usec", serviceCheckData->timestamp.tv_usec);

            NagiosObject servicecheck(neb);

            servicecheck.SetData("host_name", serviceCheckData->host_name);
            servicecheck.SetData("service_description", serviceCheckData->service_description);
            servicecheck.SetData("perf_data", nebmodule.EncodeString(serviceCheckData->perf_data));
            servicecheck.SetData("start_time", serviceCheckData->start_time.tv_sec);
            SetData("servicecheck", &servicecheck);
        }
    };

    class NagiosServiceStatusData : public NagiosObject {
    public:
        explicit NagiosServiceStatusData(INebmodule &neb, const nebstruct_service_status_data *serviceStatusData) : NagiosObject(neb) {
            SetData("type", serviceStatusData->type);
            SetData("flags", serviceStatusData->flags);
            SetData("attr", serviceStatusData->attr);
            SetData("timestamp", serviceStatusData->timestamp.tv_sec);
            SetData("timestamp_usec", serviceStatusData->timestamp.tv_usec);

            NagiosService serviceStatus(neb, reinterpret_cast<service *>(serviceStatusData->object_ptr));
            SetData("servicestatus", &serviceStatus);
        }
    };

    class NagiosStateChangeData : public NagiosObject {
    public:
        explicit NagiosStateChangeData(INebmodule &neb, const nebstruct_statechange_data *stateChangeData) : NagiosObject(neb) {
            SetData("type", stateChangeData->type);
            SetData("flags", stateChangeData->flags);
            SetData("attr", stateChangeData->attr);
            SetData("timestamp", stateChangeData->timestamp.tv_sec);
            SetData("timestamp_usec", stateChangeData->timestamp.tv_usec);

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

            NagiosObject statechange(neb);

            statechange.SetData("host_name", stateChangeData->host_name);
            statechange.SetData("service_description", stateChangeData->service_description);
            statechange.SetData("output", nebmodule.EncodeString(stateChangeData->output));
            statechange.SetData("long_output", nebmodule.EncodeString(stateChangeData->output));
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
        explicit NagiosSystemCommandData(INebmodule &neb, const nebstruct_system_command_data *systemCommandData) : NagiosObject(neb) {
            SetData("type", systemCommandData->type);
            SetData("flags", systemCommandData->flags);
            SetData("attr", systemCommandData->attr);
            SetData("timestamp", systemCommandData->timestamp.tv_sec);
            SetData("timestamp_usec", systemCommandData->timestamp.tv_usec);

            NagiosObject systemcommand(neb);

            systemcommand.SetData("command_line", systemCommandData->command_line);
            systemcommand.SetData("output", nebmodule.EncodeString(systemCommandData->output));
            systemcommand.SetData("long_output", nebmodule.EncodeString(systemCommandData->output));
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
