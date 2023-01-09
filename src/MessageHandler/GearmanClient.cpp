#include "GearmanClient.h"

#include "../Configuration.h"
#include "../Utility.h"

#include <iostream>

namespace statusengine {

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

    GearmanClient::GearmanClient(IStatusengine &se, std::shared_ptr<GearmanConfiguration> cfg)
        : MessageHandler(se), cfg(cfg), client(nullptr), worker(nullptr) {

        queueNames = cfg->GetQueueNames();
        if (!queueNames->empty()) {
            client = gearman_client_create(nullptr);
        }

        workerQueueNames = cfg->GetWorkerQueueNames();
        if (!workerQueueNames->empty()) {
            worker = gearman_worker_create(nullptr);
            gearman_worker_add_options(worker, GEARMAN_WORKER_NON_BLOCKING);
        }
    }

    GearmanClient::~GearmanClient() {
        if (client != nullptr) {
            se.Log() << "Destroy gearman client" << LogLevel::Info;
            gearman_client_free(client);
        }

        if (worker != nullptr) {
            se.Log() << "Destroy gearman worker" << LogLevel::Info;
            gearman_worker_free(worker);
            clearContainer<>(&workerContexts);
        }
    }

    bool GearmanClient::Connect() {
        if (!queueNames->empty()) {
            auto ret = gearman_client_add_servers(client, cfg->URL.c_str());
            if (gearman_success(ret)) {
                se.Log() << "Added gearman client server connection" << LogLevel::Info;
            }
            else {
                se.Log() << "Could not add client gearman server: " << gearman_client_error(client) << LogLevel::Error;
                return false;
            }
        }

        if (!workerQueueNames->empty()) {
            auto ret = gearman_worker_add_servers(worker, cfg->URL.c_str());
            if (gearman_success(ret)) {
                se.Log() << "Added gearman worker server connection" << LogLevel::Info;
            }
            else {
                se.Log() << "Could not add worker gearman server: " << gearman_client_error(client) << LogLevel::Error;
                return false;
            }
            for (auto &queue : *workerQueueNames) {
                auto ctx = new GearmanWorkerContext(queue.first, this);
                workerContexts[queue.first] = ctx;
                auto cbfn = gearman_function_create_v2(se_gearman_worker_callback);
                ret = gearman_worker_define_function(worker, queue.second.c_str(), queue.second.size(), cbfn, 0, ctx);
                if (gearman_success(ret)) {
                    se.Log() << "Added gearman worker function " << queue.second << LogLevel::Info;
                }
                else {
                    se.Log() << "Could not add gearman worker function " << queue.second << ": "
                              << gearman_client_error(client) << LogLevel::Error;
                    return false;
                }
            }
        }
        return true;
    }

    void GearmanClient::SendMessage(Queue queue, const std::string_view message) {
        auto queueName = queueNames->find(queue)->second;
        auto ret = gearman_client_do_background(client, queueName.c_str(), nullptr, message.data(), message.length(),
                                                nullptr);
        if (!gearman_success(ret)) {
            se.Log() << "Could not write message to gearman queue: " << gearman_client_error(client)
                      << LogLevel::Error;
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
                    break;
                case GEARMAN_NO_JOBS:
                    break;
                case GEARMAN_IO_WAIT:
                    gearman_worker_wait(worker);
                    moreJobs = true;
                    break;
                case GEARMAN_NO_ACTIVE_FDS:
                    se.Log() << "Gearman worker is not connected to server" << LogLevel::Error;
                    break;
                default:
                    se.Log() << "Unknown gearman worker error: " << ret << LogLevel::Error;
            }
        }
        return moreJobs;
    }

} // namespace statusengine
