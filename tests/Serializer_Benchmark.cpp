#include <gtest/gtest.h>
#include <naemon/naemon.h>
#include <cstring>
#include <gmock/gmock.h>

#include "Nebmodule.h"
#include "MessageHandler/MessageHandler.h"
#include "Statusengine.h"
#include "MessageHandler/IBulkMessageCounter.h"
#include "ObjectSerializerImpl.h"
#include "NagiosTestSuite.h"

namespace statusengine {

TEST_F(NagiosTestSuite, NagiosServiceSerializerBenchmark) {
    Nebmodule neb(nullptr, std::string(""));
    NagiosServiceSerializer serializer(neb);
    for (int i = 0; i < 100000; i++) {
        serializer.ToJson(*svc);
    }
}

}