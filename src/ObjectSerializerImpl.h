#pragma once

#include "ObjectSerializer.h"
#include <map>
#include <stdexcept>

namespace statusengine {

class NagiosProcessDataSerializer
    : public ObjectSerializer<const nebstruct_process_data &> {
  public:
    explicit NagiosProcessDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_process_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);
        SetData("pid", getpid());

        StartChildObject("processdata");
        SetData("programmname", "Naemon");
        SetData("modification_data", "removed");
        SetData("programmversion", std::string(get_program_version()));
        EndChildObject();
    }
};

class NagiosServiceSerializer : public ObjectSerializer<const service &> {
  public:
    explicit NagiosServiceSerializer(INebmodule &neb) : ObjectSerializer(neb) {}

  protected:
    void Serializer(const service &data) override {
        SetData("host_name", data.host_name);
        SetData("description", data.description);
        SetData("plugin_output", neb.EncodeString(data.plugin_output));
        SetData("long_plugin_output",
                neb.EncodeString(data.long_plugin_output));
        SetData("event_handler", data.event_handler);
        SetData("perf_data", neb.EncodeString(data.perf_data));
        SetData("check_command", data.check_command);
        SetData("check_period", data.check_period);
        SetData("current_state", data.current_state);
        SetData("has_been_checked", data.has_been_checked);
        SetData("should_be_scheduled", 1); // Nagios Compatibility
        SetData("current_attempt", data.current_attempt);
        SetData("max_attempts", data.max_attempts);
        SetData("last_check", data.last_check);
        SetData("next_check", data.next_check);
        SetData("check_type", data.check_type);
        SetData("last_state_change", data.last_state_change);
        SetData("last_hard_state_change", data.last_hard_state_change);
        SetData("last_hard_state", data.last_hard_state);
        SetData("last_time_ok", data.last_time_ok);
        SetData("last_time_warning", data.last_time_warning);
        SetData("last_time_critical", data.last_time_critical);
        SetData("last_time_unknown", data.last_time_unknown);
        SetData("state_type", data.state_type);
        SetData("last_notification", data.last_notification);
        SetData("next_notification", data.next_notification);
        SetData("no_more_notifications", data.no_more_notifications);
        SetData("notifications_enabled", data.notifications_enabled);
        SetData("problem_has_been_acknowledged",
                data.problem_has_been_acknowledged);
        SetData("acknowledgement_type", data.acknowledgement_type);
        SetData("current_notification_number",
                data.current_notification_number);
        SetData("accept_passive_checks", data.accept_passive_checks);
        SetData("event_handler_enabled", data.event_handler_enabled);
        SetData("checks_enabled", data.checks_enabled);
        SetData("flap_detection_enabled", data.flap_detection_enabled);
        SetData("is_flapping", data.is_flapping);
        SetData("percent_state_change", data.percent_state_change);
        SetData("latency", data.latency);
        SetData("execution_time", data.execution_time);
        SetData("scheduled_downtime_depth", data.scheduled_downtime_depth);
        SetData("process_performance_data", data.process_performance_data);
        SetData("obsess", data.obsess);
        SetData("modified_attributes", data.modified_attributes);
        SetData("check_interval", data.check_interval);
        SetData("retry_interval", data.retry_interval);
    }
};

class NagiosServiceCheckDataSerializer
    : public ObjectSerializer<const nebstruct_service_check_data &> {
  public:
    explicit NagiosServiceCheckDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb), macros(get_global_macros()) {}
    ~NagiosServiceCheckDataSerializer() override {
        clear_volatile_macros_r(macros);
    }

  protected:
    void
    Serializer(const nebstruct_service_check_data &serviceCheckData) override {
        SetData("type", serviceCheckData.type);
        SetData("flags", serviceCheckData.flags);
        SetData("attr", serviceCheckData.attr);
        SetData("timestamp", serviceCheckData.timestamp.tv_sec);
        SetData("timestamp_usec", serviceCheckData.timestamp.tv_usec);

        service *nag_service =
            reinterpret_cast<service *>(serviceCheckData.object_ptr);
        std::string command;
        try {
            command = commandCache.at(nag_service);
        } catch (std::out_of_range &oor) {
            char *raw_command = nullptr;
            get_raw_command_line_r(macros, nag_service->check_command_ptr,
                                   nag_service->check_command, &raw_command, 0);
            if (raw_command == nullptr) {
                command = "";
            } else {
                command = std::string(raw_command);
            }
            free(raw_command);
            commandCache[nag_service] = command;
        }

        StartChildObject("servicecheck");
        SetData("host_name", serviceCheckData.host_name);
        SetData("service_description", serviceCheckData.service_description);
        SetData("command_line", command);
        SetData("command_name", nag_service->check_command);
        SetData("output", neb.EncodeString(serviceCheckData.output));
        SetData("long_output", neb.EncodeString(serviceCheckData.long_output));
        SetData("perf_data", neb.EncodeString(serviceCheckData.perf_data));
        SetData("check_type", serviceCheckData.check_type);
        SetData("current_attempt", serviceCheckData.current_attempt);
        SetData("max_attempts", serviceCheckData.max_attempts);
        SetData("state_type", serviceCheckData.state_type);
        SetData("state", serviceCheckData.state);
        SetData("timeout", serviceCheckData.timeout);
        SetData("start_time", serviceCheckData.start_time.tv_sec);
        SetData("end_time", serviceCheckData.end_time.tv_sec);
        SetData("early_timeout", serviceCheckData.early_timeout);
        SetData("execution_time", serviceCheckData.execution_time);
        SetData("latency", serviceCheckData.latency);
        SetData("return_code", serviceCheckData.return_code);
        EndChildObject();
    }

  private:
    std::map<service *, std::string> commandCache;
    nagios_macros *macros;
};

class NagiosAcknowledgementDataSerializer
    : public ObjectSerializer<const nebstruct_acknowledgement_data &> {
  public:
    explicit NagiosAcknowledgementDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_acknowledgement_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("acknowledgement");
        SetData("host_name", data.host_name);
        SetData("service_description", data.service_description);
        SetData("author_name", data.author_name);
        SetData("comment_data", data.comment_data);
        SetData("acknowledgement_type", data.acknowledgement_type);
        SetData("state", data.state);
        SetData("is_sticky", data.is_sticky);
        SetData("persistent_comment", data.persistent_comment);
        SetData("notify_contacts", data.notify_contacts);
        EndChildObject();
    }
};

class NagiosCommentDataSerializer
    : public ObjectSerializer<const nebstruct_comment_data &> {
  public:
    explicit NagiosCommentDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_comment_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("comment");
        SetData("host_name", data.host_name);
        SetData("service_description", data.service_description);
        SetData("author_name", data.author_name);
        SetData("comment_data", data.comment_data);
        SetData("comment_type", data.comment_type);
        SetData("entry_time", data.entry_time);
        SetData("persistent", data.persistent);
        SetData("source", data.source);
        SetData("entry_type", data.entry_type);
        SetData("expires", data.expires);
        SetData("expire_time", data.expire_time);
        SetData("comment_id", data.comment_id);
        EndChildObject();
    }
};

class NagiosContactNotificationDataSerializer
    : public ObjectSerializer<const nebstruct_contact_notification_data &> {
  public:
    explicit NagiosContactNotificationDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_contact_notification_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("contactnotificationdata");
        SetData("host_name", data.host_name);
        SetData("service_description", data.service_description);
        SetData("output", neb.EncodeString(data.output));
        SetData("long_output", neb.EncodeString(data.output));
        SetData("ack_author", data.ack_author);
        SetData("ack_data", data.ack_data);
        SetData("contact_name", data.contact_name);
        SetData("state", data.state);
        SetData("reason_type", data.reason_type);
        SetData("end_time", data.end_time.tv_sec);
        SetData("start_time", data.start_time.tv_sec);
        SetData("notification_type", data.notification_type);
        EndChildObject();
    }
};

class NagiosContactNotificationMethodDataSerializer
    : public ObjectSerializer<
          const nebstruct_contact_notification_method_data &> {
  public:
    explicit NagiosContactNotificationMethodDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(
        const nebstruct_contact_notification_method_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("contactnotificationmethod");
        SetData("host_name", data.host_name);
        SetData("service_description", data.service_description);
        SetData("output", neb.EncodeString(data.output));
        SetData("ack_author", data.ack_author);
        SetData("ack_data", data.ack_data);
        SetData("contact_name", data.contact_name);
        SetData("command_name", data.command_name);
        SetData("command_args", data.command_args);
        SetData("reason_type", data.reason_type);
        SetData("state", data.state);
        SetData("start_time", data.start_time.tv_sec);
        SetData("end_time", data.end_time.tv_sec);
        EndChildObject();
    }
};

class NagiosContactStatusDataSerializer
    : public ObjectSerializer<const nebstruct_contact_status_data &> {
  public:
    explicit NagiosContactStatusDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_contact_status_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        auto tmpContact = reinterpret_cast<contact *>(data.object_ptr);

        StartChildObject("contactstatus");
        SetData("contact_name", tmpContact->name);
        SetData("host_notifications_enabled",
                tmpContact->host_notifications_enabled);
        SetData("service_notifications_enabled",
                tmpContact->service_notifications_enabled);
        SetData("last_host_notification", tmpContact->last_host_notification);
        SetData("last_service_notification",
                tmpContact->last_service_notification);
        SetData("modified_attributes", tmpContact->modified_attributes);
        SetData("modified_host_attributes",
                tmpContact->modified_host_attributes);
        SetData("modified_service_attributes",
                tmpContact->modified_service_attributes);
        EndChildObject();
    }
};

class NagiosDowntimeDataSerializer
    : public ObjectSerializer<const nebstruct_downtime_data &> {
  public:
    explicit NagiosDowntimeDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_downtime_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("downtime");
        SetData("host_name", data.host_name);
        SetData("service_description", data.service_description);
        SetData("author_name", data.author_name);
        SetData("comment_data", data.comment_data);
        SetData("host_name", data.host_name);
        SetData("downtime_type", data.downtime_type);
        SetData("entry_time", data.entry_time);
        SetData("start_time", data.start_time);
        SetData("end_time", data.end_time);
        SetData("triggered_by", data.triggered_by);
        SetData("downtime_id", data.downtime_id);
        SetData("fixed", data.fixed);
        SetData("duration", data.duration);
        EndChildObject();
    }
};

class NagiosEventHandlerDataSerializer
    : public ObjectSerializer<const nebstruct_event_handler_data &> {
  public:
    explicit NagiosEventHandlerDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_event_handler_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("eventhandler");
        SetData("host_name", data.host_name);
        SetData("service_description", data.service_description);
        SetData("output", neb.EncodeString(data.output));
        SetData("long_output", neb.EncodeString(data.output));
        SetData("command_name", data.command_name);
        SetData("command_args", data.command_args);
        SetData("command_line", data.command_line);
        SetData("state_type", data.state_type);
        SetData("state", data.state);
        SetData("timeout", data.timeout);
        SetData("early_timeout", data.early_timeout);
        SetData("return_code", data.return_code);
        SetData("execution_time", data.execution_time);
        SetData("start_time", data.start_time.tv_sec);
        SetData("end_time", data.end_time.tv_sec);
        EndChildObject();
    }
};

class NagiosExternalCommandDataSerializer
    : public ObjectSerializer<const nebstruct_external_command_data &> {
  public:
    explicit NagiosExternalCommandDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_external_command_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("externalcommand");
        SetData("command_string", data.command_string);
        SetData("command_args", data.command_args);
        SetData("command_type", data.command_type);
        SetData("entry_time", data.entry_time);
        EndChildObject();
    }
};

class NagiosFlappingDataSerializer
    : public ObjectSerializer<const nebstruct_flapping_data &> {
  public:
    explicit NagiosFlappingDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_flapping_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        comment *tmpComment = nullptr;
        if (data.flapping_type == SERVICE_FLAPPING) {
            tmpComment = find_service_comment(data.comment_id);
        } else {
            tmpComment = find_host_comment(data.comment_id);
        }

        StartChildObject("flapping");
        SetData("host_name", data.host_name);
        SetData("service_description", data.service_description);
        SetData("flapping_type", data.flapping_type);
        SetData("comment_id", data.comment_id);
        SetData("comment_entry_time",
                (tmpComment == nullptr ? 0 : tmpComment->entry_time));
        SetData("percent_change", data.percent_change);
        SetData("high_threshold", data.high_threshold);
        SetData("low_threshold", data.low_threshold);
        EndChildObject();
    }
};

class NagiosHostSerializer : public ObjectSerializer<const host &> {
  public:
    explicit NagiosHostSerializer(INebmodule &neb) : ObjectSerializer(neb) {}

  protected:
    void Serializer(const host &data) override {
        SetData("name", data.name);
        SetData("plugin_output", neb.EncodeString(data.plugin_output));
        SetData("long_plugin_output",
                neb.EncodeString(data.long_plugin_output));
        SetData("event_handler", data.event_handler);
        SetData("perf_data", neb.EncodeString(data.perf_data));
        SetData("check_command", data.check_command);
        SetData("check_period", data.check_period);
        SetData("current_state", data.current_state);
        SetData("has_been_checked", data.has_been_checked);
        SetData("should_be_scheduled", 1); // Nagios compatibility
        SetData("current_attempt", data.current_attempt);
        SetData("max_attempts", data.max_attempts);
        SetData("last_check", data.last_check);
        SetData("next_check", data.next_check);
        SetData("check_type", data.check_type);
        SetData("last_state_change", data.last_state_change);
        SetData("last_hard_state_change", data.last_hard_state_change);
        SetData("last_hard_state", data.last_hard_state);
        SetData("last_time_up", data.last_time_up);
        SetData("last_time_down", data.last_time_down);
        SetData("last_time_unreachable", data.last_time_unreachable);
        SetData("state_type", data.state_type);
        SetData("last_notification", data.last_notification);
        SetData("next_notification", data.next_notification);
        SetData("no_more_notifications", data.no_more_notifications);
        SetData("notifications_enabled", data.notifications_enabled);
        SetData("problem_has_been_acknowledged",
                data.problem_has_been_acknowledged);
        SetData("acknowledgement_type", data.acknowledgement_type);
        SetData("current_notification_number",
                data.current_notification_number);
        SetData("accept_passive_checks", data.accept_passive_checks);
        SetData("event_handler_enabled", data.event_handler_enabled);
        SetData("checks_enabled", data.checks_enabled);
        SetData("flap_detection_enabled", data.flap_detection_enabled);
        SetData("is_flapping", data.is_flapping);
        SetData("percent_state_change", data.percent_state_change);
        SetData("latency", data.latency);
        SetData("execution_time", data.execution_time);
        SetData("scheduled_downtime_depth", data.scheduled_downtime_depth);
        SetData("process_performance_data", data.process_performance_data);
        SetData("obsess", data.obsess);
        SetData("modified_attributes", data.modified_attributes);
        SetData("check_interval", data.check_interval);
        SetData("retry_interval", data.retry_interval);
    }
};

class NagiosHostCheckDataSerializer
    : public ObjectSerializer<const nebstruct_host_check_data &> {
  public:
    explicit NagiosHostCheckDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb), macros(get_global_macros()) {}
    ~NagiosHostCheckDataSerializer() override {
        clear_volatile_macros_r(macros);
    }

  protected:
    void Serializer(const nebstruct_host_check_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        host *nag_host = reinterpret_cast<host *>(data.object_ptr);
        std::string command;
        try {
            command = commandCache.at(nag_host);
        } catch (std::out_of_range &oor) {
            char *raw_command = nullptr;
            get_raw_command_line_r(macros, nag_host->check_command_ptr,
                                   nag_host->check_command, &raw_command, 0);
            if (raw_command == nullptr) {
                command = "";
            } else {
                command = std::string(raw_command);
            }
            free(raw_command);
            commandCache[nag_host] = command;
        }

        StartChildObject("hostcheck");
        SetData("host_name", data.host_name);
        SetData("command_line", command);
        SetData("command_name", nag_host->check_command);
        SetData("output", neb.EncodeString(data.output));
        SetData("long_output", neb.EncodeString(data.long_output));
        SetData("perf_data", neb.EncodeString(data.perf_data));
        SetData("check_type", data.check_type);
        SetData("current_attempt", data.current_attempt);
        SetData("max_attempts", data.max_attempts);
        SetData("state_type", data.state_type);
        SetData("state", data.state);
        SetData("timeout", data.timeout);
        SetData("start_time", data.start_time.tv_sec);
        SetData("end_time", data.end_time.tv_sec);
        SetData("early_timeout", data.early_timeout);
        SetData("execution_time", data.execution_time);
        SetData("latency", data.latency);
        SetData("return_code", data.return_code);
        EndChildObject();
    }

  private:
    std::map<host *, std::string> commandCache;
    nagios_macros *macros;
};

class NagiosHostStatusDataSerializer
    : public ObjectSerializer<const nebstruct_host_status_data &> {
  public:
    explicit NagiosHostStatusDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb), hostSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_host_status_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        InsertJsonObject("hoststatus",
                         hostSerializer.ToJson(
                             *(reinterpret_cast<host *>(data.object_ptr))));
    }

  private:
    NagiosHostSerializer hostSerializer;
};

class NagiosLogDataSerializer
    : public ObjectSerializer<const nebstruct_log_data &> {
  public:
    explicit NagiosLogDataSerializer(INebmodule &neb) : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_log_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("logentry");
        SetData("entry_time", data.entry_time);
        SetData("data_type", data.data_type);
        SetData("data", data.data);
        EndChildObject();
    }
};

class NagiosNotificationDataSerializer
    : public ObjectSerializer<const nebstruct_notification_data &> {
  public:
    explicit NagiosNotificationDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_notification_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("notification_data");
        SetData("host_name", data.host_name);
        SetData("service_description", data.service_description);
        SetData("output", neb.EncodeString(data.output));
        SetData("long_output", neb.EncodeString(data.output));
        SetData("ack_author", data.ack_author);
        SetData("ack_data", data.ack_data);
        SetData("notification_type", data.notification_type);
        SetData("start_time", data.start_time.tv_sec);
        SetData("end_time", data.end_time.tv_sec);
        SetData("reason_type", data.reason_type);
        SetData("state", data.state);
        SetData("escalated", data.escalated);
        SetData("contacts_notified", data.contacts_notified);
        EndChildObject();
    }
};

class NagiosProgramStatusDataSerializer
    : public ObjectSerializer<const nebstruct_program_status_data &> {
  public:
    explicit NagiosProgramStatusDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(const nebstruct_program_status_data &data) override {
        SetData("type", data.type);
        SetData("flags", data.flags);
        SetData("attr", data.attr);
        SetData("timestamp", data.timestamp.tv_sec);
        SetData("timestamp_usec", data.timestamp.tv_usec);

        StartChildObject("programmstatus");
        SetData("global_host_event_handler", data.global_host_event_handler);
        SetData("global_service_event_handler",
                data.global_service_event_handler);
        SetData("program_start", data.program_start);
        SetData("pid", data.pid);
        SetData("daemon_mode", data.daemon_mode);
        SetData("last_command_check", 0);
        SetData("last_log_rotation", data.last_log_rotation);
        SetData("notifications_enabled", data.notifications_enabled);
        SetData("active_service_checks_enabled",
                data.active_service_checks_enabled);
        SetData("passive_service_checks_enabled",
                data.passive_service_checks_enabled);
        SetData("active_host_checks_enabled", data.active_host_checks_enabled);
        SetData("passive_host_checks_enabled",
                data.passive_host_checks_enabled);
        SetData("event_handlers_enabled", data.event_handlers_enabled);
        SetData("flap_detection_enabled", data.flap_detection_enabled);
        SetData("failure_prediction_enabled", 0);
        SetData("process_performance_data", data.process_performance_data);
        SetData("obsess_over_hosts", data.obsess_over_hosts);
        SetData("obsess_over_services", data.obsess_over_services);
        SetData("modified_host_attributes", data.modified_host_attributes);
        SetData("modified_service_attributes",
                data.modified_service_attributes);
        EndChildObject();
    }
};

class NagiosServiceCheckPerfDataSerializer
    : public ObjectSerializer<const nebstruct_service_check_data &> {
  public:
    explicit NagiosServiceCheckPerfDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void
    Serializer(const nebstruct_service_check_data &serviceCheckData) override {
        SetData("type", serviceCheckData.type);
        SetData("flags", serviceCheckData.flags);
        SetData("attr", serviceCheckData.attr);
        SetData("timestamp", serviceCheckData.timestamp.tv_sec);
        SetData("timestamp_usec", serviceCheckData.timestamp.tv_usec);

        StartChildObject("servicecheck");
        SetData("host_name", serviceCheckData.host_name);
        SetData("service_description", serviceCheckData.service_description);
        SetData("perf_data", neb.EncodeString(serviceCheckData.perf_data));
        SetData("start_time", serviceCheckData.start_time.tv_sec);
        EndChildObject();
    }
};

class NagiosServiceStatusDataSerializer
    : public ObjectSerializer<const nebstruct_service_status_data &> {
  public:
    explicit NagiosServiceStatusDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb), serviceSerializer(neb) {}

  protected:
    void Serializer(
        const nebstruct_service_status_data &serviceStatusData) override {
        SetData("type", serviceStatusData.type);
        SetData("flags", serviceStatusData.flags);
        SetData("attr", serviceStatusData.attr);
        SetData("timestamp", serviceStatusData.timestamp.tv_sec);
        SetData("timestamp_usec", serviceStatusData.timestamp.tv_usec);

        InsertJsonObject("servicestatus",
                         serviceSerializer.ToJson(*(reinterpret_cast<service *>(
                             serviceStatusData.object_ptr))));
    }

  private:
    NagiosServiceSerializer serviceSerializer;
};

class NagiosStateChangeDataSerializer
    : public ObjectSerializer<const nebstruct_statechange_data &> {
  public:
    explicit NagiosStateChangeDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void
    Serializer(const nebstruct_statechange_data &stateChangeData) override {
        SetData("type", stateChangeData.type);
        SetData("flags", stateChangeData.flags);
        SetData("attr", stateChangeData.attr);
        SetData("timestamp", stateChangeData.timestamp.tv_sec);
        SetData("timestamp_usec", stateChangeData.timestamp.tv_usec);

        int last_state;
        int last_hard_state;

        if (stateChangeData.statechange_type == SERVICE_STATECHANGE) {
            auto tmp_service =
                reinterpret_cast<service *>(stateChangeData.object_ptr);
            last_state = tmp_service->last_state;
            last_hard_state = tmp_service->last_hard_state;
        } else {
            auto tmp_host =
                reinterpret_cast<host *>(stateChangeData.object_ptr);
            last_state = tmp_host->last_state;
            last_hard_state = tmp_host->last_hard_state;
        }

        StartChildObject("statechange");
        SetData("host_name", stateChangeData.host_name);
        SetData("service_description", stateChangeData.service_description);
        SetData("output", neb.EncodeString(stateChangeData.output));
        SetData("long_output", neb.EncodeString(stateChangeData.output));
        SetData("statechange_type", stateChangeData.statechange_type);
        SetData("state", stateChangeData.state);
        SetData("state_type", stateChangeData.state_type);
        SetData("current_attempt", stateChangeData.current_attempt);
        SetData("max_attempts", stateChangeData.max_attempts);
        SetData("last_state", last_state);
        SetData("last_hard_state", last_hard_state);
        EndChildObject();
    }
};

class NagiosSystemCommandDataSerializer
    : public ObjectSerializer<const nebstruct_system_command_data &> {
  public:
    explicit NagiosSystemCommandDataSerializer(INebmodule &neb)
        : ObjectSerializer(neb) {}

  protected:
    void Serializer(
        const nebstruct_system_command_data &systemCommandData) override {
        SetData("type", systemCommandData.type);
        SetData("flags", systemCommandData.flags);
        SetData("attr", systemCommandData.attr);
        SetData("timestamp", systemCommandData.timestamp.tv_sec);
        SetData("timestamp_usec", systemCommandData.timestamp.tv_usec);

        StartChildObject("systemcommand");
        SetData("command_line", systemCommandData.command_line);
        SetData("output", neb.EncodeString(systemCommandData.output));
        SetData("long_output", neb.EncodeString(systemCommandData.output));
        SetData("start_time", systemCommandData.start_time.tv_sec);
        SetData("end_time", systemCommandData.end_time.tv_sec);
        SetData("timeout", systemCommandData.timeout);
        SetData("early_timeout", systemCommandData.early_timeout);
        SetData("return_code", systemCommandData.return_code);
        SetData("execution_time", systemCommandData.execution_time);
        EndChildObject();
    }
};
} // namespace statusengine
