#include <doctest/doctest.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <unistd.h>

#include "test_support.h"

#include "Configuration.h"

using statusengine::Configuration;
using statusengine::FakeStatusengine;
using statusengine::LogLevel;
using statusengine::Queue;
using statusengine::WorkerQueue;

namespace {
    /// Writes a config to a temporary file and removes it again when it goes out of scope.
    class TempConfig {
      public:
        explicit TempConfig(const std::string &contents) {
            char nameTemplate[] = "/tmp/statusengine-test-XXXXXX";
            int fd = mkstemp(nameTemplate);
            REQUIRE(fd != -1);
            close(fd);
            path = nameTemplate;
            std::ofstream out(path);
            out << contents;
        }
        ~TempConfig() {
            std::remove(path.c_str());
        }
        TempConfig(const TempConfig &) = delete;
        TempConfig &operator=(const TempConfig &) = delete;

        std::string path;
    };
} // namespace

TEST_CASE("Configuration loads a minimal gearman config") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[[Gearman]]
URL = "127.0.0.1:4730"
HostStatus = "statusngin_hoststatus"
WorkerCommand = "statusngin_cmd"
)");

    REQUIRE(cfg.Load(file.path));

    auto gearmans = cfg.GetGearmanConfiguration();
    REQUIRE(gearmans->size() == 1);
    CHECK(gearmans->at(0)->URL == "127.0.0.1:4730");

    auto queues = gearmans->at(0)->GetQueueNames();
    REQUIRE(queues->size() == 1);
    CHECK(queues->at(Queue::HostStatus) == "statusngin_hoststatus");

    auto workerQueues = gearmans->at(0)->GetWorkerQueueNames();
    REQUIRE(workerQueues->size() == 1);
    CHECK(workerQueues->at(WorkerQueue::Command) == "statusngin_cmd");
}

TEST_CASE("Configuration reports a missing file instead of throwing") {
    FakeStatusengine se;
    Configuration cfg(se);

    CHECK_FALSE(cfg.Load("/nonexistent/statusengine.toml"));
    CHECK(FakeStatusengine::Logged("Could not read file"));
}

TEST_CASE("Configuration rejects a syntax error") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file("this is not = = valid toml\n");

    CHECK_FALSE(cfg.Load(file.path));
}

TEST_CASE("Configuration rejects an unknown bulk queue identifier") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[Bulk]
Queues = ["HostStatus", "ThisQueueDoesNotExist"]
)");

    CHECK_FALSE(cfg.Load(file.path));
    CHECK(FakeStatusengine::Logged("unknown queue identifier"));
}

TEST_CASE("Configuration rejects an unknown log level") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[Log]
Level = "Chatty"
)");

    CHECK_FALSE(cfg.Load(file.path));
    CHECK(FakeStatusengine::Logged("Unknown log level"));
}

TEST_CASE("Configuration reads the log level") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[Log]
Level = "Error"
)");

    REQUIRE(cfg.Load(file.path));
    CHECK(cfg.GetLogLevel() == LogLevel::Error);
}

TEST_CASE("Configuration defaults to warning level") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file("[Bulk]\nMaximum = 10\n");

    REQUIRE(cfg.Load(file.path));
    CHECK(cfg.GetLogLevel() == LogLevel::Warning);
}

TEST_CASE("Configuration applies bulk and worker defaults") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file("[Log]\nLevel = \"Info\"\n");

    REQUIRE(cfg.Load(file.path));
    CHECK(cfg.GetBulkMaximum() == 200ul);
    CHECK(cfg.GetBulkFlushInterval() == 10);
    CHECK(cfg.GetMaxWorkerMessagesPerInterval() == 1000000ul);
    CHECK(cfg.GetMaxWorkerRuntime() == std::chrono::milliseconds(100));
    CHECK(cfg.GetStartupScheduleMax() == 0);
}

TEST_CASE("Configuration reads bulk settings and queue membership") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[Bulk]
Maximum = 50
FlushInterval = 3
Queues = ["HostStatus", "ServiceCheck"]

[Scheduler]
StartupScheduleMax = 30

[Worker]
MaxWorkerMessagesPerInterval = 42
MaxRuntimeMilliseconds = 250
)");

    REQUIRE(cfg.Load(file.path));
    CHECK(cfg.GetBulkMaximum() == 50ul);
    CHECK(cfg.GetBulkFlushInterval() == 3);
    CHECK(cfg.GetStartupScheduleMax() == 30);
    CHECK(cfg.GetMaxWorkerMessagesPerInterval() == 42ul);
    CHECK(cfg.GetMaxWorkerRuntime() == std::chrono::milliseconds(250));
    CHECK(cfg.IsBulkQueue(Queue::HostStatus));
    CHECK(cfg.IsBulkQueue(Queue::ServiceCheck));
    CHECK_FALSE(cfg.IsBulkQueue(Queue::LogData));
}

TEST_CASE("Rabbitmq needs a hostname") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[[Rabbitmq]]
Username = "statusengine"
)");

    CHECK_FALSE(cfg.Load(file.path));
    CHECK(FakeStatusengine::Logged("specify a hostname"));
}

TEST_CASE("Rabbitmq queues and exchange are durable by default") {
    // RabbitMQ 4 refuses to declare a queue that is neither durable nor exclusive, so this
    // default is what keeps the broker able to connect at all.
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[[Rabbitmq]]
Hostname = "localhost"
Password = "statusengine"
HostStatus = "statusngin_hoststatus"
)");

    REQUIRE(cfg.Load(file.path));
    auto rabbits = cfg.GetRabbitmqConfiguration();
    REQUIRE(rabbits->size() == 1);
    CHECK(rabbits->at(0)->DurableQueues);
    CHECK(rabbits->at(0)->DurableExchange);
}

TEST_CASE("Rabbitmq durability can be turned off explicitly") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[[Rabbitmq]]
Hostname = "localhost"
Password = "statusengine"
DurableQueues = false
DurableExchange = false
)");

    REQUIRE(cfg.Load(file.path));
    auto rabbits = cfg.GetRabbitmqConfiguration();
    REQUIRE(rabbits->size() == 1);
    CHECK_FALSE(rabbits->at(0)->DurableQueues);
    CHECK_FALSE(rabbits->at(0)->DurableExchange);
}

TEST_CASE("Rabbitmq applies connection defaults") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[[Rabbitmq]]
Hostname = "rabbit"
Password = "statusengine"
)");

    REQUIRE(cfg.Load(file.path));
    auto rfg = cfg.GetRabbitmqConfiguration()->at(0);
    CHECK(rfg->Port == 5672);
    CHECK(rfg->Vhost == "/");
    CHECK(rfg->Username == "statusengine");
    CHECK(rfg->Exchange == "statusengine");
    CHECK(rfg->Timeout.tv_sec == 30);
    CHECK_FALSE(rfg->SSL);
    CHECK(rfg->SSLVerify);
}

TEST_CASE("Multiple connections of the same type are all loaded") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file(R"(
[[Gearman]]
URL = "a:4730"
HostStatus = "q1"

[[Gearman]]
URL = "b:4730"
HostStatus = "q2"
)");

    REQUIRE(cfg.Load(file.path));
    REQUIRE(cfg.GetGearmanConfiguration()->size() == 2);
    CHECK(cfg.GetGearmanConfiguration()->at(0)->URL == "a:4730");
    CHECK(cfg.GetGearmanConfiguration()->at(1)->URL == "b:4730");
}

TEST_CASE("The shipped example configuration parses") {
    // statusengine.toml is what users copy, so it has to stay loadable. This is also the
    // regression guard for the toml11 v1 -> v4 migration.
    FakeStatusengine se;
    Configuration cfg(se);

    REQUIRE(cfg.Load(STATUSENGINE_EXAMPLE_CONFIG));

    // The example enables gearman with a set of queues and marks several of them bulk.
    REQUIRE(cfg.GetGearmanConfiguration()->size() == 1);
    auto queues = cfg.GetGearmanConfiguration()->at(0)->GetQueueNames();
    CHECK(queues->at(Queue::HostStatus) == "statusngin_hoststatus");
    CHECK(cfg.IsBulkQueue(Queue::HostStatus));
    CHECK(cfg.GetStartupScheduleMax() == 30);
}

TEST_CASE("The developer environment configuration parses") {
    FakeStatusengine se;
    Configuration cfg(se);

    REQUIRE(cfg.Load(STATUSENGINE_DEVENV_CONFIG));
    CHECK(cfg.GetGearmanConfiguration()->size() == 1);
    CHECK(cfg.GetRabbitmqConfiguration()->size() == 1);
}


TEST_CASE("A worker runtime of zero is accepted and means unbounded") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file("[Worker]\nMaxRuntimeMilliseconds = 0\n");

    REQUIRE(cfg.Load(file.path));
    CHECK(cfg.GetMaxWorkerRuntime() == std::chrono::milliseconds::zero());
}

TEST_CASE("An invalid worker runtime is rejected rather than ignored") {
    FakeStatusengine se;
    Configuration cfg(se);
    TempConfig file("[Worker]\nMaxRuntimeMilliseconds = \"soon\"\n");

    CHECK_FALSE(cfg.Load(file.path));
    CHECK(FakeStatusengine::Logged("MaxRuntimeMilliseconds"));
}
