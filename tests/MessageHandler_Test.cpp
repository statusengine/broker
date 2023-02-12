#include "MessageHandlerTestSuite.h"

namespace statusengine {

TEST_F(MessageHandlerTestSuite, ProcessMessage) {
    EmptyMessageHandler msg(se);
    msg.ProcessMessage(WorkerQueue::WorkerOCSP,
                       testMessageProcessSerivceCheck());
}

} // namespace statusengine
