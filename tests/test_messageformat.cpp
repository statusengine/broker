#include <doctest/doctest.h>

#include <cstring>
#include <fstream>
#include <set>
#include <sstream>
#include <string>

#include "test_support.h"

#include "NagiosObject.h"

using namespace statusengine;

namespace {

    /// Reads a recorded broker message from tests/fixtures. The document owns the
    /// values, so it is kept alive by the caller.
    yyjson_doc *LoadFixture(const std::string &name) {
        std::ifstream in(std::string(STATUSENGINE_FIXTURE_DIR) + "/" + name);
        REQUIRE_MESSAGE(in.good(), "missing fixture: ", name);
        std::stringstream buffer;
        buffer << in.rdbuf();
        std::string text = buffer.str();
        yyjson_doc *doc = yyjson_read(text.c_str(), text.length(), 0);
        REQUIRE_MESSAGE(doc != nullptr, "fixture is not valid json: ", name);
        return doc;
    }

    std::set<std::string> KeysOf(yyjson_val *obj) {
        std::set<std::string> keys;
        REQUIRE(yyjson_is_obj(obj));
        size_t idx, max;
        yyjson_val *key, *value;
        yyjson_obj_foreach(obj, idx, max, key, value) {
            (void)value;
            keys.insert(std::string(yyjson_get_str(key), yyjson_get_len(key)));
        }
        return keys;
    }

    /// Unwraps a fixture down to the single recorded message, bulk or not.
    yyjson_val *FirstMessage(yyjson_val *fixture) {
        yyjson_val *messages = yyjson_obj_get(fixture, "messages");
        if (messages != nullptr) {
            REQUIRE(yyjson_arr_size(messages) > 0);
            return yyjson_arr_get(messages, 0);
        }
        return fixture;
    }

    std::string Rendered(NagiosObject &obj) {
        return obj.ToString();
    }

    /**
     * Compares a freshly built message against a recorded one: the outer envelope and the
     * named sub object have to carry exactly the same keys. Values are not compared, the
     * fixtures were recorded from a different host.
     */
    void CheckShape(NagiosObject &produced, const std::string &fixtureName, const char *subObject) {
        yyjson_doc *fixture = LoadFixture(fixtureName);
        yyjson_val *expected = FirstMessage(yyjson_doc_get_root(fixture));

        std::string renderedText = Rendered(produced);
        yyjson_doc *actualDoc = yyjson_read(renderedText.c_str(), renderedText.length(), 0);
        REQUIRE(actualDoc != nullptr);
        yyjson_val *actual = yyjson_doc_get_root(actualDoc);

        CHECK(KeysOf(actual) == KeysOf(expected));

        if (subObject != nullptr) {
            yyjson_val *actualSub = yyjson_obj_get(actual, subObject);
            yyjson_val *expectedSub = yyjson_obj_get(expected, subObject);
            REQUIRE(actualSub != nullptr);
            REQUIRE(expectedSub != nullptr);
            CHECK(KeysOf(actualSub) == KeysOf(expectedSub));
        }

        yyjson_doc_free(actualDoc);
        yyjson_doc_free(fixture);
    }

    /// Fills the fields every nebstruct shares.
    template <typename T>
    void FillHeader(T &data, int type) {
        std::memset(&data, 0, sizeof(data));
        data.type = type;
        data.flags = 0;
        data.attr = 0;
        data.timestamp.tv_sec = 1785470668;
        data.timestamp.tv_usec = 701779;
    }

    char *Str(const char *s) {
        return const_cast<char *>(s);
    }

} // namespace

TEST_CASE("host check message keeps its shape") {
    host nagHost;
    std::memset(&nagHost, 0, sizeof(nagHost));
    nagHost.name = Str("localhost");
    nagHost.check_command = Str("check-host-alive");

    nebstruct_host_check_data data;
    FillHeader(data, NEBTYPE_HOSTCHECK_PROCESSED);
    data.host_name = Str("localhost");
    data.object_ptr = &nagHost;
    data.output = Str("PING OK - Packet loss = 0%, RTA = 0.08 ms");
    data.long_output = Str("");
    data.perf_data = Str("rta=0.078000ms;3000.000000;5000.000000;0.000000");

    NagiosHostCheckData msg(&data);
    CheckShape(msg, "statusngin_hostchecks.json", "hostcheck");
}

TEST_CASE("service check message keeps its shape") {
    service nagService;
    std::memset(&nagService, 0, sizeof(nagService));
    nagService.host_name = Str("localhost");
    nagService.description = Str("Uptime");
    nagService.check_command = Str("check_uptime");

    nebstruct_service_check_data data;
    FillHeader(data, NEBTYPE_SERVICECHECK_PROCESSED);
    data.host_name = Str("localhost");
    data.service_description = Str("Uptime");
    data.object_ptr = &nagService;
    data.output = Str("OK - up 3 days");
    data.long_output = Str("");
    data.perf_data = Str("uptime=3");

    NagiosServiceCheckData msg(&data);
    CheckShape(msg, "statusngin_servicechecks.json", "servicecheck");
}

TEST_CASE("service perfdata message keeps its shape") {
    nebstruct_service_check_data data;
    FillHeader(data, NEBTYPE_SERVICECHECK_PROCESSED);
    data.host_name = Str("localhost");
    data.service_description = Str("Uptime");
    data.perf_data = Str("uptime=3");

    NagiosServiceCheckPerfData msg(&data);
    CheckShape(msg, "statusngin_service_perfdata.json", "servicecheck");
}

TEST_CASE("host status message keeps its shape") {
    host nagHost;
    std::memset(&nagHost, 0, sizeof(nagHost));
    nagHost.name = Str("localhost");
    nagHost.plugin_output = Str("PING OK");
    nagHost.check_command = Str("check-host-alive");

    nebstruct_host_status_data data;
    FillHeader(data, NEBTYPE_HOSTSTATUS_UPDATE);
    data.object_ptr = &nagHost;

    NagiosHostStatusData msg(&data);
    CheckShape(msg, "statusngin_hoststatus.json", "hoststatus");
}

TEST_CASE("service status message keeps its shape") {
    service nagService;
    std::memset(&nagService, 0, sizeof(nagService));
    nagService.host_name = Str("localhost");
    nagService.description = Str("Uptime");
    nagService.plugin_output = Str("OK");

    nebstruct_service_status_data data;
    FillHeader(data, NEBTYPE_SERVICESTATUS_UPDATE);
    data.object_ptr = &nagService;

    NagiosServiceStatusData msg(&data);
    CheckShape(msg, "statusngin_servicestatus.json", "servicestatus");
}

TEST_CASE("state change message keeps its shape") {
    service nagService;
    std::memset(&nagService, 0, sizeof(nagService));
    nagService.last_state = 1;
    nagService.last_hard_state = 1;

    nebstruct_statechange_data data;
    FillHeader(data, NEBTYPE_STATECHANGE_END);
    data.host_name = Str("localhost");
    data.service_description = Str("Flapping");
    data.statechange_type = SERVICE_STATECHANGE;
    data.object_ptr = &nagService;
    data.output = Str("Your random value is: 158");

    NagiosStateChangeData msg(&data);
    CheckShape(msg, "statusngin_statechanges.json", "statechange");
}

TEST_CASE("log entry message keeps its shape") {
    nebstruct_log_data data;
    FillHeader(data, NEBTYPE_LOG_DATA);
    data.entry_time = 1785470668;
    data.data_type = 262144;
    data.data = Str("Event broker module initialized successfully.");

    NagiosLogData msg(&data);
    CheckShape(msg, "statusngin_logentries.json", "logentry");
}

TEST_CASE("notification message keeps its shape") {
    nebstruct_notification_data data;
    FillHeader(data, NEBTYPE_NOTIFICATION_END);
    data.host_name = Str("localhost");
    data.service_description = Str("Uptime");
    data.output = Str("CRITICAL");
    data.contacts_notified = 1;

    NagiosNotificationData msg(&data);
    CheckShape(msg, "statusngin_notifications.json", "notification_data");
}

TEST_CASE("acknowledgement message keeps its shape") {
    nebstruct_acknowledgement_data data;
    FillHeader(data, NEBTYPE_ACKNOWLEDGEMENT_ADD);
    data.host_name = Str("localhost");
    data.author_name = Str("admin");
    data.comment_data = Str("looking into it");

    NagiosAcknowledgementData msg(&data);
    CheckShape(msg, "statusngin_acknowledgements.json", "acknowledgement");
}

TEST_CASE("acknowledgement carries the end time") {
    nebstruct_acknowledgement_data data;
    FillHeader(data, NEBTYPE_ACKNOWLEDGEMENT_ADD);
    data.host_name = Str("localhost");
    data.author_name = Str("admin");
    data.comment_data = Str("until tomorrow");
#ifndef BUILD_NAGIOS
    data.end_time = 1785470668;
#endif

    NagiosAcknowledgementData msg(&data);
    std::string renderedText = Rendered(msg);
    yyjson_doc *parsedDoc = yyjson_read(renderedText.c_str(), renderedText.length(), 0);
    REQUIRE(parsedDoc != nullptr);

    yyjson_val *ack = yyjson_obj_get(yyjson_doc_get_root(parsedDoc), "acknowledgement");
    REQUIRE(ack != nullptr);
    // The key is always present, so consumers can rely on it regardless of the core.
    yyjson_val *endTime = yyjson_obj_get(ack, "end_time");
    REQUIRE(endTime != nullptr);

#ifndef BUILD_NAGIOS
    CHECK(yyjson_get_sint(endTime) == 1785470668);
#else
    // Nagios has no expiring acknowledgements, so 0 - "does not expire" - is not a
    // placeholder here but the truth for every nagios acknowledgement.
    CHECK(yyjson_get_sint(endTime) == 0);
#endif

    yyjson_doc_free(parsedDoc);
}

TEST_CASE("downtime message keeps its shape") {
    nebstruct_downtime_data data;
    FillHeader(data, NEBTYPE_DOWNTIME_ADD);
    data.host_name = Str("localhost");
    data.author_name = Str("admin");
    data.comment_data = Str("maintenance");

    NagiosDowntimeData msg(&data);
    CheckShape(msg, "statusngin_downtimes.json", "downtime");
}

TEST_CASE("contact notification method message keeps its shape") {
    nebstruct_contact_notification_method_data data;
    FillHeader(data, NEBTYPE_CONTACTNOTIFICATIONMETHOD_END);
    data.host_name = Str("localhost");
    data.contact_name = Str("admin");
    data.command_name = Str("notify-by-email");
    data.output = Str("CRITICAL");

    NagiosContactNotificationMethodData msg(&data);
    CheckShape(msg, "statusngin_contactnotificationmethod.json", "contactnotificationmethod");
}

TEST_CASE("notifications without notified contacts stay empty") {
    // The broker drops these on purpose, see NagiosNotificationData. An empty object is
    // how MessageQueueHandler recognises a filtered event.
    nebstruct_notification_data data;
    FillHeader(data, NEBTYPE_NOTIFICATION_END);
    data.host_name = Str("localhost");
    data.contacts_notified = 0;

    NagiosNotificationData msg(&data);
    CHECK(msg.isEmpty());
}

TEST_CASE("notification start events are dropped") {
    nebstruct_notification_data data;
    FillHeader(data, 600); // NEBTYPE_NOTIFICATION_START
    data.host_name = Str("localhost");
    data.contacts_notified = 5;

    NagiosNotificationData msg(&data);
    CHECK(msg.isEmpty());
}

TEST_CASE("a populated message is not empty") {
    nebstruct_log_data data;
    FillHeader(data, NEBTYPE_LOG_DATA);
    data.data = Str("something happened");

    NagiosLogData msg(&data);
    CHECK_FALSE(msg.isEmpty());
}

TEST_CASE("non utf8 plugin output is converted in the message") {
    // The whole point of the encoder: a latin1 check output has to reach the queue as
    // valid utf8, otherwise the JSON is undecodable for the worker.
    const std::string latin1 = "Gr\xf6\xdf" "e des Puffers \xfc" "berschritten, Dienst l\xe4uft nicht";

    nebstruct_service_check_data data;
    FillHeader(data, NEBTYPE_SERVICECHECK_PROCESSED);
    data.host_name = Str("localhost");
    data.service_description = Str("Uptime");
    data.perf_data = const_cast<char *>(latin1.c_str());

    NagiosServiceCheckPerfData msg(&data);
    std::string rendered = Rendered(msg);

    // A successful round trip through the parser is what proves the conversion
    // happened: invalid utf8 would not survive it intact.
    yyjson_doc *parsedDoc = yyjson_read(rendered.c_str(), rendered.length(), 0);
    REQUIRE(parsedDoc != nullptr);

    yyjson_val *sub = yyjson_obj_get(yyjson_doc_get_root(parsedDoc), "servicecheck");
    REQUIRE(sub != nullptr);
    yyjson_val *perf = yyjson_obj_get(sub, "perf_data");
    REQUIRE(perf != nullptr);
    CHECK(std::string(yyjson_get_str(perf)) == "Größe des Puffers überschritten, Dienst läuft nicht");

    yyjson_doc_free(parsedDoc);
}

TEST_CASE("core restart message keeps its shape") {
    // Until this was added the broker only sent object_type, so the recorded shape and the
    // produced one disagreed and this was the one message type without a golden test.
    nebstruct_process_data data;
    FillHeader(data, NEBTYPE_PROCESS_START);

    NagiosRestartData msg(&data);
    CheckShape(msg, "statusngin_core_restart.json", nullptr);
}

TEST_CASE("core restart carries naemon's event time") {
    nebstruct_process_data data;
    FillHeader(data, NEBTYPE_PROCESS_START);
    data.timestamp.tv_sec = 1785470668;

    NagiosRestartData msg(&data);
    std::string renderedText = Rendered(msg);
    yyjson_doc *parsedDoc = yyjson_read(renderedText.c_str(), renderedText.length(), 0);
    REQUIRE(parsedDoc != nullptr);
    yyjson_val *parsed = yyjson_doc_get_root(parsedDoc);

    yyjson_val *value = yyjson_obj_get(parsed, "object_type");
    REQUIRE(value != nullptr);
    CHECK(yyjson_get_int(value) == NEBTYPE_PROCESS_RESTART);

    value = yyjson_obj_get(parsed, "timestamp");
    REQUIRE(value != nullptr);
    // The worker reads 0 as "not set", so a real restart must never report 0.
    CHECK(yyjson_get_sint(value) == 1785470668);

    yyjson_doc_free(parsedDoc);
}
