#pragma once

#include "gmock/gmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "INebmodule.h"
#include "IStatusengine.h"
#include "LogStream.h"
#include "MessageHandler/MessageHandler.h"
#include "NagiosTestSuite.h"

namespace statusengine {
class MockStatusengine : public IStatusengine {
  public:
    MOCK_METHOD(INebmodule &, GetNebmodule, (), (override));
    MOCK_METHOD(int, Init, (), (override));
    MOCK_METHOD(void, FlushBulkQueue, (), (override));
    MOCK_METHOD(IMessageHandlerList *, GetMessageHandler, (), (const override));
    MOCK_METHOD(void, InitEventCallbacks, (), (override));
    MOCK_METHOD(time_t, GetStartupScheduleMax, (), (const override));
    MOCK_METHOD(int, Callback, (int, void *), (override));
    MOCK_METHOD(LogStream &, Log, (), (override));
};

class MockNebmodule : public INebmodule {

    MOCK_METHOD(void, Init, (), (override));
    MOCK_METHOD(int, Callback, (int, void *), (override));
    MOCK_METHOD(bool, RegisterCallback, (NEBCallbackType), (override));
    MOCK_METHOD(void, RegisterEventCallback, (EventCallback *), (override));
    MOCK_METHOD(void, ScheduleHostCheckDelay, (host *, time_t), (override));
    MOCK_METHOD(void, ScheduleHostCheckFixed, (host *, time_t), (override));
    MOCK_METHOD(void, ScheduleServiceCheckDelay, (service *, time_t),
                (override));
    MOCK_METHOD(void, ScheduleServiceCheckFixed, (service *, time_t),
                (override));
    MOCK_METHOD(void, DeleteDowntime,
                (const char *, const char *, time_t, time_t, const char *),
                (override));
    MOCK_METHOD(void, ProcessExternalCommand, (char *cmd), (override));
    MOCK_METHOD(Neb_NebmodulePtr, GetNebNebmodulePtr, (), (override));
    MOCK_METHOD(std::string, EncodeString, (char *), (override));
};

class MessageHandlerTestSuite : public NagiosTestSuite {

  public:
    void SetUp() override {
        NagiosTestSuite::SetUp();

        EXPECT_CALL(se, Log)
            .Times(::testing::AnyNumber())
            .WillRepeatedly([this]() -> LogStream & { return this->ls; });
        EXPECT_CALL(se, GetNebmodule)
            .Times(::testing::AnyNumber())
            .WillRepeatedly([this]() -> INebmodule & { return this->neb; });
    }

  protected:
    MockNebmodule neb;
    MockStatusengine se;
    LogStream ls;
};

inline const char *testMessageProcessSerivceCheck() {
    return R"MAJESTY(
{
    "messages": {
        [
            "host_name": "test",
            "service_description": "test",
            "output": "this is a test",
            "long_output": "more output",
            "perf_data": "c=1",
            "check_type": 1,
            "return_code": 0,
            "start_time": 0,
            "end_time": 0,
            "early_timeout": 0,
            "latency": 0.0,
            "exited_ok": 1
        ]
    }
}
)MAJESTY";
}

class EmptyMessageHandler : public MessageHandler {
  public:
    explicit EmptyMessageHandler(IStatusengine &se) : MessageHandler(se) {}

    virtual bool Connect() { return true; }
    virtual bool Worker(unsigned long &) { return true; }
    virtual void SendMessage(Queue, const std::string_view) {}
};
} // namespace statusengine