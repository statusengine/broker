#include <doctest/doctest.h>

#include <chrono>
#include <memory>
#include <vector>

#include "test_support.h"

#include "MessageHandler/MessageHandlerList.h"

using statusengine::FakeClock;
using statusengine::FakeIoWaitHandler;
using statusengine::FakeMessageHandler;
using statusengine::FakeStatusengine;
using statusengine::IMessageHandler;
using statusengine::MessageHandlerList;
using statusengine::OverloadReporter;
using statusengine::WorkerRunResult;

namespace {
    using HandlerList = std::vector<std::shared_ptr<IMessageHandler>>;

    std::shared_ptr<FakeMessageHandler> Add(HandlerList &list, unsigned long messages, bool keepAsking) {
        auto handler = std::make_shared<FakeMessageHandler>(messages, keepAsking);
        list.push_back(handler);
        return handler;
    }

    /// Everything that is not about the time budget runs without one.
    WorkerRunResult RunUnbounded(HandlerList &handlers, unsigned long maxMessages) {
        return MessageHandlerList::RunWorkers(handlers, maxMessages, std::chrono::milliseconds::zero());
    }
} // namespace

TEST_CASE("a handler that asks for more without progressing does not spin the loop") {
    // Always "there is more", never a processed message, so the message counter can never
    // end the loop. Unbounded this ran forever, inside naemon's event loop.
    HandlerList handlers;
    auto stuck = Add(handlers, 0, true);

    RunUnbounded(handlers, 1000000ul);

    CHECK(stuck->calls == MessageHandlerList::maxRoundsWithoutProgress);
}

TEST_CASE("a worker that waits for its socket before every message still drains") {
    // The regression this file exists for. A gearman worker reports GEARMAN_IO_WAIT once
    // per job, so the very first round of a tick usually processes nothing. Ending the
    // loop on that round cut throughput to roughly one message per tick: measured against
    // a real job server, 50000 queued jobs went from being drained in a single tick to 48
    // messages in 50 ticks, and the queue grew without bound.
    HandlerList handlers;
    auto handler = std::make_shared<FakeIoWaitHandler>(500);
    handlers.push_back(handler);

    RunUnbounded(handlers, 1000000ul);

    CHECK(handler->remaining == 0);
}

TEST_CASE("waiting for the socket does not consume the no progress budget") {
    // Progress resets the budget, so a handler alternating wait and message can keep going
    // indefinitely - which is exactly what draining a full queue looks like.
    HandlerList handlers;
    auto handler = std::make_shared<FakeIoWaitHandler>(4);
    handlers.push_back(handler);

    RunUnbounded(handlers, 1000000ul);

    // four waits, four messages, and the round that finds the queue empty
    CHECK(handler->calls == 9);
}

TEST_CASE("a handler drains its queue within one tick") {
    HandlerList handlers;
    auto busy = Add(handlers, 5, false);

    RunUnbounded(handlers, 1000000ul);

    CHECK(busy->remaining == 0);
    // five rounds that processed something, plus the one that found nothing left
    CHECK(busy->calls == 6);
}

TEST_CASE("the message limit is respected") {
    HandlerList handlers;
    auto busy = Add(handlers, 100, false);

    RunUnbounded(handlers, 10ul);

    CHECK(busy->calls == 10);
    CHECK(busy->remaining == 90);
}

TEST_CASE("a stuck handler does not stop the others") {
    HandlerList handlers;
    auto stuck = Add(handlers, 0, true);
    auto busy = Add(handlers, 3, false);

    RunUnbounded(handlers, 1000000ul);

    // The loop keeps going while anyone makes progress, so the working handler drains...
    CHECK(busy->remaining == 0);
    // ...and once nobody does, the no progress budget ends it rather than the stuck one.
    CHECK(stuck->calls == 3 + MessageHandlerList::maxRoundsWithoutProgress);
}

TEST_CASE("a handler that goes quiet ends the loop") {
    HandlerList handlers;
    auto first = Add(handlers, 2, false);
    auto second = Add(handlers, 1, false);

    RunUnbounded(handlers, 1000000ul);

    CHECK(first->remaining == 0);
    CHECK(second->remaining == 0);
}

TEST_CASE("an empty handler list terminates") {
    HandlerList handlers;
    RunUnbounded(handlers, 1000000ul);
    CHECK(handlers.empty());
}

TEST_CASE("a zero message limit still runs one round") {
    // maxMessages is checked after a round, so work already done is never thrown away.
    HandlerList handlers;
    auto busy = Add(handlers, 5, false);

    RunUnbounded(handlers, 0ul);

    CHECK(busy->calls == 1);
    CHECK(busy->remaining == 4);
}


// ---------------------------------------------------------------------------------------
// The time budget. Without it one call was measured holding naemon's event loop for 128
// seconds while it applied a backlog of 100000 check results.
// ---------------------------------------------------------------------------------------

TEST_CASE("the loop stops on its time budget and says so") {
    FakeClock::Reset(std::chrono::milliseconds(10));
    HandlerList handlers;
    auto busy = Add(handlers, 1000, false);

    // The deadline costs one reading, then one per round: rounds see 10, 20, 30, 40 and
    // 50ms, and the fifth reaches the 50ms budget.
    auto result = MessageHandlerList::RunWorkers<FakeClock>(handlers, 1000000ul, std::chrono::milliseconds(50));

    CHECK(result.budgetExhausted);
    CHECK(result.WorkRemaining());
    CHECK(result.processed == 5);
    CHECK(busy->remaining == 995);
}

TEST_CASE("the clock is read once per round") {
    FakeClock::Reset(std::chrono::milliseconds(10));
    HandlerList handlers;
    Add(handlers, 1000, false);

    auto result = MessageHandlerList::RunWorkers<FakeClock>(handlers, 1000000ul, std::chrono::milliseconds(50));

    // one for the deadline, one per round
    CHECK(FakeClock::readings == result.processed + 1);
}

TEST_CASE("a handler that runs dry before the budget reports no work remaining") {
    // This one matters: work remaining is what makes the core reschedule immediately, so
    // an idle broker reporting it would spin instead of waiting for its next interval.
    FakeClock::Reset(std::chrono::milliseconds(1));
    HandlerList handlers;
    auto busy = Add(handlers, 3, false);

    auto result = MessageHandlerList::RunWorkers<FakeClock>(handlers, 1000000ul, std::chrono::milliseconds(1000));

    CHECK_FALSE(result.budgetExhausted);
    CHECK_FALSE(result.WorkRemaining());
    CHECK(busy->remaining == 0);
}

TEST_CASE("a budget of zero means unbounded") {
    FakeClock::Reset(std::chrono::milliseconds(1000));
    HandlerList handlers;
    auto busy = Add(handlers, 50, false);

    auto result = MessageHandlerList::RunWorkers<FakeClock>(handlers, 1000000ul, std::chrono::milliseconds::zero());

    CHECK(busy->remaining == 0);
    CHECK_FALSE(result.budgetExhausted);
    // Not consulted at all, so a huge step cannot end the loop early.
    CHECK(FakeClock::readings == 0ul);
}

TEST_CASE("a run that spends its whole budget without progress is not work remaining") {
    // What an unreachable job server looks like: libgearman reports GEARMAN_IO_WAIT round
    // after round, each costing a 10ms poll, so the budget is gone before the no progress
    // guard is reached. Rescheduling immediately would retry a broken connection ten times
    // a second, and reporting an overload would blame the queues for a connection problem.
    FakeClock::Reset(std::chrono::milliseconds(10));
    HandlerList handlers;
    auto stuck = Add(handlers, 0, true);

    auto result = MessageHandlerList::RunWorkers<FakeClock>(handlers, 1000000ul, std::chrono::milliseconds(50));

    CHECK(result.processed == 0ul);
    CHECK_FALSE(result.budgetExhausted);
    CHECK_FALSE(result.WorkRemaining());
    // Stopped on the budget, well before the 16 round guard would have ended it.
    CHECK(stuck->calls == 5);
}

TEST_CASE("the message limit also counts as work remaining") {
    HandlerList handlers;
    Add(handlers, 100, false);

    auto result = RunUnbounded(handlers, 10ul);

    CHECK(result.messageLimitReached);
    CHECK(result.WorkRemaining());
    CHECK_FALSE(result.budgetExhausted);
}

TEST_CASE("a round without progress is not work remaining") {
    // A handler that asks for more without progressing must not make the core reschedule
    // immediately - that would spin the event loop rather than wait for the next tick.
    HandlerList handlers;
    Add(handlers, 0, true);

    auto result = RunUnbounded(handlers, 1000000ul);

    CHECK_FALSE(result.WorkRemaining());
}

// ---------------------------------------------------------------------------------------
// Overload reporting
// ---------------------------------------------------------------------------------------

namespace {
    WorkerRunResult Stopped(unsigned long processed) {
        WorkerRunResult r;
        r.processed = processed;
        r.budgetExhausted = true;
        return r;
    }

    WorkerRunResult Drained() {
        WorkerRunResult r;
        r.processed = 7;
        return r;
    }
} // namespace

TEST_CASE("an overload is reported when it starts, on an interval, and when it ends") {
    FakeStatusengine se;
    OverloadReporter reporter(se);

    reporter.Report(Stopped(100), 1000);
    CHECK(FakeStatusengine::Logged("Worker stopped with messages still queued"));
    CHECK(FakeStatusengine::Logged("its time budget"));

    capturedLogs.clear();
    // A backlog hits the budget on every run; those must not each produce a line.
    for (time_t t = 1001; t < 1000 + OverloadReporter::reportIntervalSeconds; ++t) {
        reporter.Report(Stopped(100), t);
    }
    CHECK(capturedLogs.empty());

    reporter.Report(Stopped(100), 1000 + OverloadReporter::reportIntervalSeconds);
    CHECK(FakeStatusengine::Logged("Worker stopped with messages still queued"));

    capturedLogs.clear();
    reporter.Report(Drained(), 1200);
    CHECK(FakeStatusengine::Logged("Worker queues are empty again"));
}

TEST_CASE("no overload is reported while the queues keep up") {
    FakeStatusengine se;
    OverloadReporter reporter(se);

    for (time_t t = 0; t < 500; ++t) {
        reporter.Report(Drained(), t);
    }

    CHECK(capturedLogs.empty());
}

TEST_CASE("hitting the message limit is reported as such") {
    FakeStatusengine se;
    OverloadReporter reporter(se);

    WorkerRunResult result;
    result.processed = 10;
    result.messageLimitReached = true;
    reporter.Report(result, 1000);

    CHECK(FakeStatusengine::Logged("its message limit"));
}
