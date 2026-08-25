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

    /// Reads a recorded broker message from tests/fixtures.
    json_object *LoadFixture(const std::string &name) {
        std::ifstream in(std::string(STATUSENGINE_FIXTURE_DIR) + "/" + name);
        REQUIRE_MESSAGE(in.good(), "missing fixture: ", name);
        std::stringstream buffer;
        buffer << in.rdbuf();
        json_object *obj = json_tokener_parse(buffer.str().c_str());
        REQUIRE_MESSAGE(obj != nullptr, "fixture is not valid json: ", name);
        return obj;
    }

    std::set<std::string> KeysOf(json_object *obj) {
        std::set<std::string> keys;
        REQUIRE(json_object_is_type(obj, json_type_object));
        json_object_object_foreach(obj, key, value) {
            (void)value;
            keys.insert(key);
        }
        return keys;
    }

    /// Unwraps a fixture down to the single recorded message, bulk or not.
    json_object *FirstMessage(json_object *fixture) {
        json_object *messages;
        if (json_object_object_get_ex(fixture, "messages", &messages)) {
            REQUIRE(json_object_array_length(messages) > 0);
            return json_object_array_get_idx(messages, 0);
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
        json_object *fixture = LoadFixture(fixtureName);
        json_object *expected = FirstMessage(fixture);

        json_object *actual = json_tokener_parse(Rendered(produced).c_str());
        REQUIRE(actual != nullptr);

        CHECK(KeysOf(actual) == KeysOf(expected));

        if (subObject != nullptr) {
            json_object *actualSub = nullptr;
            json_object *expectedSub = nullptr;
            REQUIRE(json_object_object_get_ex(actual, subObject, &actualSub));
            REQUIRE(json_object_object_get_ex(expected, subObject, &expectedSub));
            CHECK(KeysOf(actualSub) == KeysOf(expectedSub));
        }

        json_object_put(actual);
        json_object_put(fixture);
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

    // json-c refuses to build a string from invalid utf8, so a successful round trip
    // through the parser is what proves the conversion happened.
    json_object *parsed = json_tokener_parse(rendered.c_str());
    REQUIRE(parsed != nullptr);

    json_object *sub = nullptr;
    REQUIRE(json_object_object_get_ex(parsed, "servicecheck", &sub));
    json_object *perf = nullptr;
    REQUIRE(json_object_object_get_ex(sub, "perf_data", &perf));
    CHECK(std::string(json_object_get_string(perf)) == "Größe des Puffers überschritten, Dienst läuft nicht");

    json_object_put(parsed);
}
