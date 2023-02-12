#include <cstring>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <naemon/naemon.h>

#include "MessageHandler/IBulkMessageCounter.h"
#include "MessageHandler/MessageHandler.h"
#include "NagiosTestSuite.h"
#include "Nebmodule.h"
#include "ObjectSerializerImpl.h"
#include "Statusengine.h"

namespace statusengine {

TEST_F(NagiosTestSuite, NagiosServiceStatusDataSerializer) {
    Nebmodule neb(nullptr, std::string(""));
    NagiosServiceStatusDataSerializer serializer(neb);
    nebstruct_service_status_data data;
    data.type = 1;
    data.flags = 0;
    data.attr = 0;
    data.timestamp.tv_sec = std::time(nullptr);
    data.timestamp.tv_usec = 0;
    data.object_ptr = svc;
    auto out = serializer.ToJson(data);
    ASSERT_NE(out.find(TARGET_SERVICE_NAME), std::string::npos);
    ASSERT_NE(out.find(TARGET_HOST_NAME), std::string::npos);
}

TEST_F(NagiosTestSuite, NagiosProcessDataSerializer) {
    Nebmodule neb(nullptr, std::string(""));
    NagiosProcessDataSerializer serializer(neb);
    nebstruct_process_data data;
    data.type = 1;
    data.flags = 0;
    data.attr = 0;
    data.timestamp.tv_sec = std::time(nullptr);
    data.timestamp.tv_usec = 0;
    serializer.ToJson(data);
    auto out = serializer.ToJson(data);
}

class FakeMessageHandler : public MessageHandler {
  public:
    FakeMessageHandler(IStatusengine &se) : MessageHandler(se) {}

    virtual bool Connect() { return true; };
    virtual bool Worker(unsigned long &) { return true; };
    virtual void SendMessage(Queue, const std::string_view){};
};

/*
json_object *check_result_service() {
    json_object *obj = json_object_new_object();
    json_object_object_add(obj, "exited_ok", json_object_new_int64(1));
    json_object_object_add(obj, "early_timeout", json_object_new_int64(0));
    json_object_object_add(obj, "start_time",
                           json_object_new_int64(std::time(nullptr) - 2));
    json_object_object_add(obj, "end_time",
                           json_object_new_int64(std::time(nullptr)));
    json_object_object_add(obj, "return_code", json_object_new_int64(0));
    json_object_object_add(obj, "check_type", json_object_new_int64(0));
    json_object_object_add(obj, "latency", json_object_new_double(0.01));
    json_object_object_add(obj, "host_name",
                           json_object_new_string(TARGET_HOST_NAME));
    json_object_object_add(obj, "service_description",
                           json_object_new_string(TARGET_SERVICE_NAME));
    json_object_object_add(obj, "output",
                           json_object_new_string("this is an example output"));

    json_object *main = json_object_new_object();
    json_object_object_add(main, "Command",
                           json_object_new_string("check_result"));
    json_object_object_add(main, "Data", obj);
    return main;
}

TEST_F(NagiosTestSuite, ParseScheduleCheck) {
    json_object *main = check_result_service();
    Nebmodule neb(nullptr, std::string(""));
    statusengine::IStatusengine &se = neb.GetStatusengine();
    FakeMessageHandler handler(se);
    handler.ProcessMessage(WorkerQueue::WorkerCommand, main);
    json_object_put(main);
}

TEST_F(NagiosTestSuite, ParseScheduleCheckBenchmark) {
    json_object *main = check_result_service();
    Nebmodule neb(nullptr, std::string(""));
    statusengine::IStatusengine &se = neb.GetStatusengine();
    FakeMessageHandler handler(se);
    for (int i = 0; i < 100000; i++) {
        handler.ProcessMessage(WorkerQueue::WorkerCommand, main);
    }
    json_object_put(main);
}
*/
class MockBulkCounter : public IBulkMessageCounter {
  public:
    MOCK_METHOD(void, IncrementCounter, (), (override));
};

TEST_F(NagiosTestSuite, MessageQueueHandler) {
    Nebmodule neb(nullptr, std::string(""));
    statusengine::IStatusengine &se = neb.GetStatusengine();
    std::shared_ptr<IMessageHandler> handler =
        std::make_shared<FakeMessageHandler>(se);
    auto msgHndl =
        std::make_shared<std::vector<std::shared_ptr<IMessageHandler>>>();
    msgHndl->push_back(handler);
    MockBulkCounter counter;
    MessageQueueHandler qHandler(se, counter, Queue::ServiceCheck, msgHndl,
                                 true);
    EXPECT_CALL(counter, IncrementCounter).Times(::testing::Exactly(5));
    int i = 0;
    ON_CALL(counter, IncrementCounter).WillByDefault([&i, &qHandler]() {
        if (i++ % 3 == 0) {
            qHandler.FlushBulkQueue();
        }
    });
    qHandler.SendMessage("{\"this\": \"test\"}");
    qHandler.SendMessage("{\"this\": \"test\"}");
    qHandler.SendMessage("{\"this\": \"test\"}");
    qHandler.SendMessage("{\"this\": \"test\"}");
    qHandler.SendMessage("{\"this\": \"test\"}");
}

} // namespace statusengine
