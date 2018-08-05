#include "MessageHandlerConfiguration.h"

#include <string>

#include "Configuration.h"
#include "Statusengine.h"

namespace statusengine {
    MessageHandlerConfiguration::MessageHandlerConfiguration(Statusengine *se) : se(se) {
        queues = std::make_shared<std::map<Queue, std::string>>();
        workerQueues = std::make_shared<std::map<WorkerQueue, std::string>>();
    }

    bool MessageHandlerConfiguration::InitLoad(const toml::Table &tbl) {
        for (auto &tableEntry : tbl) {
            auto qName = Configuration::QueueName.find(tableEntry.first);
            if (qName != Configuration::QueueName.end()) {
                try {
                    (*queues)[qName->second] = toml::get<std::string>(tableEntry.second);
                }
                catch (const toml::type_error &tte) {
                    se->Log() << "Invalid configuration: Invalid value for key " << tableEntry.first << LogLevel::Error;
                    return false;
                }
            }
            else {
                auto wqName = Configuration::WorkerQueueName.find(tableEntry.first);
                if (wqName != Configuration::WorkerQueueName.end()) {
                    try {
                        (*workerQueues)[wqName->second] = toml::get<std::string>(tableEntry.second);
                    }
                    catch (const toml::type_error &tte) {
                        se->Log() << "Invalid configuration: Invalid value for key " << tableEntry.first
                                  << LogLevel::Error;
                        return false;
                    }
                }
            }
        }

        return Load(tbl);
    }

    const std::shared_ptr<std::map<Queue, std::string>> MessageHandlerConfiguration::GetQueueNames() const {
        return queues;
    }

    std::shared_ptr<std::set<Queue>> MessageHandlerConfiguration::GetQueues() const {
        auto queueIds = std::make_shared<std::set<Queue>>();
        for (auto &queue : *queues) {
            queueIds->insert(queue.first);
        }
        return queueIds;
    }

    const std::shared_ptr<std::map<WorkerQueue, std::string>> MessageHandlerConfiguration::GetWorkerQueueNames() const {
        return workerQueues;
    }

    std::shared_ptr<std::set<WorkerQueue>> MessageHandlerConfiguration::GetWorkerQueues() const {
        auto queueIds = std::make_shared<std::set<WorkerQueue>>();
        for (auto &queue : *workerQueues) {
            queueIds->insert(queue.first);
        }
        return queueIds;
    }

} // namespace statusengine
