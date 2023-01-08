#include <gtest/gtest.h>
#include <naemon/naemon.h>

#include "NagiosObject.h"
#include "Nebmodule.h"

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

}
