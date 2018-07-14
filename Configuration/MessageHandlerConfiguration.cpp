#include "MessageHandlerConfiguration.h"

#include <string>

#include "Configuration.h"
#include "Statusengine.h"

namespace statusengine {
    MessageHandlerConfiguration::MessageHandlerConfiguration(Statusengine *se) : se(se) {
        queues = std::make_shared<std::map<Queue, std::string>>();
    }

    bool MessageHandlerConfiguration::InitLoad(const toml::Table &tbl) {
        for (auto it = tbl.begin(); it != tbl.end(); ++it) {
            auto itQueueName = Configuration::QueueName.find(it->first);
            if (itQueueName != Configuration::QueueName.end()) {
                try {
                    (*queues)[itQueueName->second] = toml::get<std::string>(it->second);
                }
                catch (const toml::type_error &tte) {
                    se->Log() << "Invalid configuration: Invalid value for key " << it->first << LogLevel::Error;
                    return false;
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
        for (auto it = queues->begin(); it != queues->end(); ++it) {
            queueIds->insert(it->first);
        }
        return queueIds;
    }

} // namespace statusengine
