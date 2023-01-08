#include <gtest/gtest.h>
#include <naemon/naemon.h>
#include <cstring>

#include "NagiosObject.h"
#include "Nebmodule.h"
#include "MessageHandler/MessageHandler.h"
#include "Statusengine.h"

namespace statusengine {


#define TARGET_SERVICE_NAME "my_service"
#define TARGET_HOST_NAME "my_host"

class NagiosTestSuite : public ::testing::Test {
    public:
   
    void SetUp() override {
        init_event_queue();
        init_objects_host(1);
        init_objects_service(1);
        init_objects_command(1);

        cmd = create_command("my_command", "/bin/true");
        ASSERT_NE(cmd, nullptr);
        register_command(cmd);

        hst = create_host(TARGET_HOST_NAME);
        ASSERT_NE(hst, nullptr);
        hst->check_command_ptr = cmd;
        hst->check_command = static_cast<char *>(strdup("something or other"));
        register_host(hst);

        svc = create_service(hst, TARGET_SERVICE_NAME);
        ASSERT_NE(svc, nullptr);
        svc->check_command_ptr = cmd;
        svc->accept_passive_checks = TRUE;
        register_service(svc);
    }

    void TearDown() override {
        destroy_event_queue();
        destroy_objects_command();
        destroy_objects_service(true);
        destroy_objects_host();
    }

    host *hst;
    service *svc;
    command *cmd;
};


TEST_F(NagiosTestSuite, NagiosServiceBenchmark) {
    Nebmodule neb(nullptr, std::string(""));
    for (int i = 0; i < 100000; i++) {
        NagiosService nag(neb, svc);
        nag.ToString();
    }
}

TEST_F(NagiosTestSuite, NagiosService) {
    Nebmodule neb(nullptr, std::string(""));
    NagiosService nag(neb, svc);
    auto out = nag.ToString();
    ASSERT_NE(out.find(TARGET_SERVICE_NAME), std::string::npos);
    ASSERT_NE(out.find(TARGET_HOST_NAME), std::string::npos);
}


TEST_F(NagiosTestSuite, NagiosHostBenchmark) {
    Nebmodule neb(nullptr, std::string(""));
    for (int i = 0; i < 100000; i++) {
        NagiosHost nag(neb, hst);
        nag.ToString();
    }
}

TEST_F(NagiosTestSuite, NagiosHost) {
    Nebmodule neb(nullptr, std::string(""));
    NagiosHost nag(neb, hst);
    auto out = nag.ToString();
    ASSERT_EQ(out.find(TARGET_SERVICE_NAME), std::string::npos);
    ASSERT_NE(out.find(TARGET_HOST_NAME), std::string::npos);
}


/*
                if (jsonKey.compare("host_name") == 0) {
                    cr.host_name = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("service_description") == 0) {
                    cr.service_description = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("output") == 0) {
                    output = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("long_output") == 0) {
                    longOutput = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("perf_data") == 0) {
                    perfData = get_json_string(jsonValue);
                }

*/

class FakeMessageHandler : public MessageHandler {
    public:
    FakeMessageHandler(IStatusengine &se) : MessageHandler(se) {}

    virtual bool Connect() {return true;};
    virtual bool Worker(unsigned long &) {return true;};
    virtual void SendMessage(Queue, const std::string &) {};
};


json_object* check_result_service() {
    json_object *obj = json_object_new_object();
    json_object_object_add(obj, "exited_ok", json_object_new_int64(0));
    json_object_object_add(obj, "early_timeout", json_object_new_int64(0));
    json_object_object_add(obj, "start_time", json_object_new_int64(std::time(nullptr)-2));
    json_object_object_add(obj, "end_time", json_object_new_int64(std::time(nullptr)));
    json_object_object_add(obj, "return_code", json_object_new_int64(0));
    json_object_object_add(obj, "check_type", json_object_new_int64(0));
    json_object_object_add(obj, "latency", json_object_new_double(0.01));
    json_object_object_add(obj, "host_name", json_object_new_string(TARGET_HOST_NAME));
    json_object_object_add(obj, "service_description", json_object_new_string(TARGET_SERVICE_NAME));
    json_object_object_add(obj, "output", json_object_new_string("this is an example output"));

    json_object *main = json_object_new_object();
    json_object_object_add(main, "Command", json_object_new_string("check_result"));
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

}
