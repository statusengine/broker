#include "MessageHandlerTestSuite.h"

namespace statusengine {

TEST_F(MessageHandlerTestSuite, ProcessMessage) {
    EmptyMessageHandler msg(se);
    for (int i = 0; i < 100000; i++) {
        msg.ProcessMessage(WorkerQueue::WorkerOCSP,
                           testMessageProcessSerivceCheck());
    }
}

} // namespace statusengine
