#pragma once

#include "MessageHandler.h"

#include <ctime>

#include "libgearman-1.0/gearman.h"
#include "libgearman-1.0/strerror.h"

namespace statusengine {
    class GearmanConfiguration;
    class GearmanWorkerContext;

    class GearmanClient : public MessageHandler {
      public:
        explicit GearmanClient(IStatusengine *se, std::shared_ptr<GearmanConfiguration> cfg);
        ~GearmanClient();

        bool Connect() override;
        bool Worker(unsigned long &counter) override;

        void SendMessage(Queue queue, const std::string &message) override;

        virtual gearman_return_t WorkerCallback(WorkerQueue queue, gearman_job_st *job);

      private:
        std::shared_ptr<GearmanConfiguration> cfg;

        gearman_client_st *client;
        gearman_worker_st *worker;

        std::shared_ptr<std::map<Queue, std::string>> queueNames;
        std::shared_ptr<std::map<WorkerQueue, std::string>> workerQueueNames;
        std::map<WorkerQueue, std::unique_ptr<GearmanWorkerContext>> workerContexts;

        // An unreachable job server is one condition, not one condition per message. These
        // track it so an outage is reported once at each end instead of on every attempt.
        unsigned long failedSends;
        unsigned long failedWorkerCalls;
        gearman_return_t lastWorkerError;
        time_t lastSendErrorReport;
        time_t lastWorkerErrorReport;

        /// Reports a worker error once per distinct cause, counting the rest.
        void ReportWorkerError(gearman_return_t ret);
        /// Reports that the worker is healthy again, if it previously was not.
        void ReportWorkerRecovered();
    };
} // namespace statusengine
