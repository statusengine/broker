#pragma once

#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <map>

#include "vendor/toml.hpp"

#include "Queue.h"
#include "IStatusengine.h"

namespace statusengine {
    class RabbitmqConfiguration;
    class GearmanConfiguration;

    class Configuration {
      public:
        explicit Configuration(IStatusengine *se);
        ~Configuration();
        bool Load(std::string);

        time_t GetBulkFlushInterval() const;
        unsigned long GetBulkMaximum() const;
        bool isBulkQueue(Queue queue) const;
        time_t GetStartupScheduleMax() const;

        std::vector<std::shared_ptr<GearmanConfiguration>> *GetGearmanConfiguration();
        std::vector<std::shared_ptr<RabbitmqConfiguration>> *GetRabbitmqConfiguration();
        virtual unsigned long GetMaxWorkerMessagesPerInterval() const;

        static const std::map<std::string, Queue> QueueName;
        static const std::map<Queue, std::string> QueueId;
        static const std::map<std::string, WorkerQueue> WorkerQueueName;
        static const std::map<WorkerQueue, std::string> WorkerQueueId;

      private:
        IStatusengine *se;
        toml::Table cfg;
        toml::Table bulkTable;
        toml::Table schedulerTable;

        std::vector<std::shared_ptr<RabbitmqConfiguration>> rabbitmq;
        std::vector<std::shared_ptr<GearmanConfiguration>> gearman;
        std::set<Queue> bulkQueues;

        unsigned long maxWorkerMessagesPerInterval;

        template <typename T> T GetTomlDefault(const toml::Table &tab, const char *ky, T &&opt) const {
            try {
                return toml::get_or(tab, ky, opt);
            }
            catch (const toml::type_error &tte) {
                se->Log() << "Invalid configuration: Invalid value for key " << ky << LogLevel::Error;
            }
            return opt;
        }
    };
} // namespace statusengine
