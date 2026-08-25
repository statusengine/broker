#include <doctest/doctest.h>

#include <set>
#include <string>

#include "Queue.h"

using statusengine::Queue;
using statusengine::QueueNameHandler;
using statusengine::WorkerQueue;

TEST_CASE("Queue names and ids are complete and consistent") {
    const auto &byName = QueueNameHandler::Instance().QueueNames();
    const auto &byId = QueueNameHandler::Instance().QueueIds();

    // Both directions have to describe the same set, otherwise a queue can be configured
    // but never resolved back to its name (or the other way around).
    REQUIRE(byName.size() == byId.size());
    CHECK(byId.size() == 23);

    for (const auto &entry : byName) {
        REQUIRE(byId.count(entry.second) == 1);
        CHECK(byId.at(entry.second) == entry.first);
    }
    for (const auto &entry : byId) {
        REQUIRE(byName.count(entry.second) == 1);
        CHECK(byName.at(entry.second) == entry.first);
    }
}

TEST_CASE("Worker queue names and ids are complete and consistent") {
    const auto &byName = QueueNameHandler::Instance().WorkerQueueNames();
    const auto &byId = QueueNameHandler::Instance().WorkerQueueIds();

    REQUIRE(byName.size() == byId.size());
    CHECK(byId.size() == 3);

    for (const auto &entry : byName) {
        REQUIRE(byId.count(entry.second) == 1);
        CHECK(byId.at(entry.second) == entry.first);
    }
}

TEST_CASE("Known identifiers keep their spelling") {
    // These strings are the public configuration format, they must not drift.
    const auto &byId = QueueNameHandler::Instance().QueueIds();
    CHECK(byId.at(Queue::HostStatus) == "HostStatus");
    CHECK(byId.at(Queue::ServicePerfData) == "ServicePerfData");
    CHECK(byId.at(Queue::ContactNotificationMethodData) == "ContactNotificationMethodData");
    CHECK(byId.at(Queue::OCSP) == "OCSP");
    CHECK(byId.at(Queue::OCHP) == "OCHP");

    const auto &workerById = QueueNameHandler::Instance().WorkerQueueIds();
    CHECK(workerById.at(WorkerQueue::Command) == "WorkerCommand");
    CHECK(workerById.at(WorkerQueue::OCSP) == "WorkerOCSP");
    CHECK(workerById.at(WorkerQueue::OCHP) == "WorkerOCHP");
}

TEST_CASE("Queue and worker queue namespaces are separate") {
    // OCSP exists in both enums with different identifiers; mixing them up would silently
    // route messages to the wrong queue.
    const auto &queueNames = QueueNameHandler::Instance().QueueNames();
    const auto &workerNames = QueueNameHandler::Instance().WorkerQueueNames();

    for (const auto &entry : workerNames) {
        CHECK(queueNames.count(entry.first) == 0);
    }
    CHECK(queueNames.count("OCSP") == 1);
    CHECK(workerNames.count("WorkerOCSP") == 1);
}
