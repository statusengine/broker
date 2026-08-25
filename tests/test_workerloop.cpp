#include <doctest/doctest.h>

#include <memory>
#include <vector>

#include "test_support.h"

#include "MessageHandler/MessageHandlerList.h"

using statusengine::FakeMessageHandler;
using statusengine::IMessageHandler;
using statusengine::MessageHandlerList;

namespace {
    using HandlerList = std::vector<std::shared_ptr<IMessageHandler>>;

    std::shared_ptr<FakeMessageHandler> Add(HandlerList &list, unsigned long messages, bool keepAsking) {
        auto handler = std::make_shared<FakeMessageHandler>(messages, keepAsking);
        list.push_back(handler);
        return handler;
    }
} // namespace

TEST_CASE("a handler that asks for more without progressing does not spin the loop") {
    // This is the GEARMAN_IO_WAIT shape: always "there is more", never a processed
    // message, so the message counter can never end the loop. Before the loop was bounded
    // by progress this ran forever, inside naemon's event loop.
    HandlerList handlers;
    auto stuck = Add(handlers, 0, true);

    MessageHandlerList::RunWorkers(handlers, 1000000ul);

    CHECK(stuck->calls == 1);
}

TEST_CASE("a handler drains its queue within one tick") {
    HandlerList handlers;
    auto busy = Add(handlers, 5, false);

    MessageHandlerList::RunWorkers(handlers, 1000000ul);

    CHECK(busy->remaining == 0);
    // five rounds that processed something, plus the one that found nothing left
    CHECK(busy->calls == 6);
}

TEST_CASE("the message limit is respected") {
    HandlerList handlers;
    auto busy = Add(handlers, 100, false);

    MessageHandlerList::RunWorkers(handlers, 10ul);

    CHECK(busy->calls == 10);
    CHECK(busy->remaining == 90);
}

TEST_CASE("a stuck handler does not stop the others") {
    HandlerList handlers;
    auto stuck = Add(handlers, 0, true);
    auto busy = Add(handlers, 3, false);

    MessageHandlerList::RunWorkers(handlers, 1000000ul);

    // The loop keeps going while anyone makes progress, so the working handler drains...
    CHECK(busy->remaining == 0);
    // ...and it ends on the first round where nobody did, rather than on the stuck one.
    CHECK(stuck->calls == 4);
}

TEST_CASE("a handler that goes quiet ends the loop") {
    HandlerList handlers;
    auto first = Add(handlers, 2, false);
    auto second = Add(handlers, 1, false);

    MessageHandlerList::RunWorkers(handlers, 1000000ul);

    CHECK(first->remaining == 0);
    CHECK(second->remaining == 0);
}

TEST_CASE("an empty handler list terminates") {
    HandlerList handlers;
    MessageHandlerList::RunWorkers(handlers, 1000000ul);
    CHECK(handlers.empty());
}

TEST_CASE("a zero message limit still runs one round") {
    // maxMessages is checked after a round, so work already done is never thrown away.
    HandlerList handlers;
    auto busy = Add(handlers, 5, false);

    MessageHandlerList::RunWorkers(handlers, 0ul);

    CHECK(busy->calls == 1);
    CHECK(busy->remaining == 4);
}
