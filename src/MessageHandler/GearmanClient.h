#pragma once

#include "MessageHandler.h"

#include "libgearman-1.0/gearman.h"

namespace statusengine {
    class GearmanConfiguration;
    class GearmanWorkerContext;

    class GearmanClient : public MessageHandler {
      public:
        explicit GearmanClient(IStatusengine &se, std::shared_ptr<GearmanConfiguration> cfg);
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
        std::map<WorkerQueue, GearmanWorkerContext *> workerContexts;
    };
} // namespace statusengine
