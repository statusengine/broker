#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Queue.h"
#include "vendor/toml.hpp"

namespace statusengine {
    class Statusengine;

    class MessageHandlerConfiguration {
      public:
        bool InitLoad(const toml::Table &tbl);
        virtual bool Load(const toml::Table &tbl) = 0;

        virtual const std::shared_ptr<std::map<Queue, std::string>> GetQueueNames() const;
        virtual std::shared_ptr<std::set<Queue>> GetQueues() const;
        virtual const std::shared_ptr<std::map<WorkerQueue, std::string>> GetWorkerQueueNames() const;
        virtual std::shared_ptr<std::set<WorkerQueue>> GetWorkerQueues() const;

      protected:
        MessageHandlerConfiguration(Statusengine *se);

        Statusengine *se;

        std::shared_ptr<std::map<Queue, std::string>> queues;
        std::shared_ptr<std::map<WorkerQueue, std::string>> workerQueues;
        unsigned long maxWorkerMessagesPerInterval;
    };
} // namespace statusengine