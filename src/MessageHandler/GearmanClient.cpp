#include "GearmanClient.h"

#include "Configuration.h"

#include <iostream>

namespace statusengine {

    namespace {
        /// Upper bound in milliseconds for a single gearman_worker_wait() poll.
        const int gearmanPollTimeoutMs = 10;

        /// How often an ongoing outage is repeated in the log. Often enough that a broken
        /// job server cannot go unnoticed, rarely enough that it does not drown the log:
        /// once per message was around 4 lines a second even on a tiny installation.
        const time_t outageReportIntervalSeconds = 300;

        /// True if this condition should be written to the log now.
        bool ShouldReport(time_t &lastReport, unsigned long occurrences) {
            const time_t now = std::time(nullptr);
            if (occurrences == 1 || now - lastReport >= outageReportIntervalSeconds) {
                lastReport = now;
                return true;
            }
            return false;
        }
    } // namespace

    class GearmanWorkerContext {
      public:
        GearmanWorkerContext(WorkerQueue queue, GearmanClient *client) : Queue(queue), Client(client){};
        WorkerQueue Queue;
        GearmanClient *Client;
    };

    gearman_return_t se_gearman_worker_callback(gearman_job_st *job, void *worker_context) {
        auto ctx = reinterpret_cast<GearmanWorkerContext *>(worker_context);
        auto ret = ctx->Client->WorkerCallback(ctx->Queue, job);
        return ret;
    }

    GearmanClient::GearmanClient(IStatusengine *se, std::shared_ptr<GearmanConfiguration> cfg)
        : MessageHandler(se), cfg(cfg), client(nullptr), worker(nullptr), failedSends(0),
          failedWorkerCalls(0), lastWorkerError(GEARMAN_SUCCESS), lastSendErrorReport(0),
          lastWorkerErrorReport(0) {

        queueNames = cfg->GetQueueNames();
        if (!queueNames->empty()) {
            client = gearman_client_create(nullptr);
        }

        workerQueueNames = cfg->GetWorkerQueueNames();
        if (!workerQueueNames->empty()) {
            worker = gearman_worker_create(nullptr);
            gearman_worker_add_options(worker, GEARMAN_WORKER_NON_BLOCKING);
            // gearman_worker_wait() polls with this timeout. libgearman defaults to -1,
            // an unbounded poll() - inside naemon's event loop that means an unresponsive
            // job server can stall the whole monitoring core. Waiting for a healthy local
            // server was measured at 3 to 119 microseconds, so this cap is only ever
            // reached when something is wrong, and a timed out poll simply retries on the
            // next worker tick.
            gearman_worker_set_timeout(worker, gearmanPollTimeoutMs);
        }
    }

    GearmanClient::~GearmanClient() {
        if (client != nullptr) {
            se->Log() << "Destroy gearman client" << LogLevel::Info;
            gearman_client_free(client);
        }

        if (worker != nullptr) {
            se->Log() << "Destroy gearman worker" << LogLevel::Info;
            gearman_worker_free(worker);
            workerContexts.clear();
        }
    }

    bool GearmanClient::Connect() {
        if (!queueNames->empty()) {
            auto ret = gearman_client_add_servers(client, cfg->URL.c_str());
            if (gearman_success(ret)) {
                se->Log() << "Added gearman client server connection" << LogLevel::Info;
            }
            else {
                se->Log() << "Could not add client gearman server: " << gearman_client_error(client) << LogLevel::Error;
                return false;
            }
        }

        if (!workerQueueNames->empty()) {
            auto ret = gearman_worker_add_servers(worker, cfg->URL.c_str());
            if (gearman_success(ret)) {
                se->Log() << "Added gearman worker server connection" << LogLevel::Info;
            }
            else {
                se->Log() << "Could not add worker gearman server: " << gearman_client_error(client) << LogLevel::Error;
                return false;
            }
            for (auto &queue : *workerQueueNames) {
                // The raw pointer is handed to libgearman as the callback context; the
                // context object itself stays owned by workerContexts.
                std::unique_ptr<GearmanWorkerContext> ownedCtx(new GearmanWorkerContext(queue.first, this));
                auto ctx = ownedCtx.get();
                workerContexts[queue.first] = std::move(ownedCtx);
                auto cbfn = gearman_function_create_v2(se_gearman_worker_callback);
                ret = gearman_worker_define_function(worker, queue.second.c_str(), queue.second.size(), cbfn, 0, ctx);
                if (gearman_success(ret)) {
                    se->Log() << "Added gearman worker function " << queue.second << LogLevel::Info;
                }
                else {
                    se->Log() << "Could not add gearman worker function " << queue.second << ": "
                              << gearman_client_error(client) << LogLevel::Error;
                    return false;
                }
            }
        }
        return true;
    }

    void GearmanClient::SendMessage(Queue queue, const std::string &message) {
        auto queueNameIt = queueNames->find(queue);
        if (queueNameIt == queueNames->end()) {
            const auto &QueueId = QueueNameHandler::Instance().QueueIds();
            se->Log() << "No gearman queue configured for " << QueueId.at(queue) << ", dropping message"
                      << LogLevel::Error;
            return;
        }
        auto ret = gearman_client_do_background(client, queueNameIt->second.c_str(), nullptr, message.c_str(),
                                                message.length(), nullptr);
        if (!gearman_success(ret)) {
            // An unreachable job server fails every single message. Logging each one buries
            // the rest of the log and can outpace the events themselves, so report the
            // outage once and count what it costs.
            if (ShouldReport(lastSendErrorReport, ++failedSends)) {
                auto error = gearman_client_error(client);
                se->Log() << "Could not write message to gearman queue (" << failedSends
                          << " failed send(s) so far): " << (error != nullptr ? error : "unknown error")
                          << LogLevel::Error;
            }
        }
        else if (failedSends > 0) {
            // Deliberately "failed send(s)" and not "messages lost": this counts the
            // attempts libgearman reported an error for, which is not necessarily
            // everything that failed to reach the job server.
            se->Log() << "Gearman queue is writable again after " << failedSends << " failed send(s)"
                      << LogLevel::Error;
            failedSends = 0;
            lastSendErrorReport = 0;
        }
    }

    void GearmanClient::ReportWorkerError(gearman_return_t ret) {
        ++failedWorkerCalls;
        // A job server that stays down would otherwise produce a line per worker tick,
        // around the clock. Report a new cause immediately, and an ongoing one on an
        // interval so it cannot be silently forgotten.
        if (ret != lastWorkerError) {
            failedWorkerCalls = 1;
            lastWorkerError = ret;
        }
        if (ShouldReport(lastWorkerErrorReport, failedWorkerCalls)) {
            se->Log() << "Gearman worker error (" << failedWorkerCalls << " failed attempt(s) so far): "
                      << gearman_strerror(ret) << LogLevel::Error;
        }
    }

    void GearmanClient::ReportWorkerRecovered() {
        if (failedWorkerCalls > 0) {
            se->Log() << "Gearman worker is working again after " << failedWorkerCalls << " failed attempt(s)"
                      << LogLevel::Error;
            failedWorkerCalls = 0;
            lastWorkerError = GEARMAN_SUCCESS;
            lastWorkerErrorReport = 0;
        }
    }

    gearman_return_t GearmanClient::WorkerCallback(WorkerQueue queue, gearman_job_st *job) {
        std::string msg(static_cast<const char *>(gearman_job_workload(job)), gearman_job_workload_size(job));
        ProcessMessage(queue, msg);
        return GEARMAN_SUCCESS;
    }

    bool GearmanClient::Worker(unsigned long &counter) {
        bool moreJobs = false;
        if (!workerQueueNames->empty()) {
            auto ret = gearman_worker_work(worker);
            switch (ret) {
                case GEARMAN_SUCCESS:
                    ++counter;
                    moreJobs = true;
                    ReportWorkerRecovered();
                    break;
                case GEARMAN_NO_JOBS:
                    ReportWorkerRecovered();
                    break;
                case GEARMAN_IO_WAIT:
                    // Not just a sleep: gearman_wait() runs the poll() that refreshes the
                    // connection's readiness. Without it libgearman never learns the socket
                    // became writable and keeps returning IO_WAIT forever, so the worker
                    // never finishes its PRE_SLEEP handshake and never picks up a job.
                    // It is bounded by the timeout set in the constructor.
                    gearman_worker_wait(worker);
                    moreJobs = true;
                    break;
                default:
                    // Every remaining code means the worker could not do its job -
                    // no servers, name resolution, a lost connection. gearman_strerror()
                    // names them, which the previous "Unknown ... : 12" did not.
                    ReportWorkerError(ret);
            }
        }
        return moreJobs;
    }

} // namespace statusengine
