#include "MessageHandlerList.h"

#include "Configuration/Configuration.h"
#include "Configuration/GearmanConfiguration.h"
#include "Configuration/RabbitmqConfiguration.h"

#ifdef WITH_GEARMAN
#include "GearmanClient.h"
#endif

#ifdef WITH_RABBITMQ
#include "RabbitmqClient.h"
#endif

namespace statusengine {
    MessageHandlerList::MessageHandlerList(Statusengine *se, Configuration *cfg)
        : se(se), maxBulkSize(0), globalBulkCounter(0) {
        maxBulkSize = cfg->GetBulkMaximum();
        std::map<Queue, std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>>> handlers;
        auto InsertHandler = [&handlers](Queue queue, std::shared_ptr<MessageHandler> handler) {
            std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> queueHandlers;
            try {
                queueHandlers = handlers.at(queue);
            }
            catch (std::out_of_range &oor) {
                queueHandlers = std::make_shared<std::vector<std::shared_ptr<MessageHandler>>>();
                handlers[queue] = queueHandlers;
            }
            queueHandlers->push_back(handler);
        };

#ifdef WITH_GEARMAN
        auto gearmanConfig = cfg->GetGearmanConfiguration();
        for (auto it = gearmanConfig->begin(); it != gearmanConfig->end(); ++it) {
            auto queues = (*it)->GetQueues();
            for (auto queue = queues->begin(); queue != queues->end(); ++queue) {
                InsertHandler(*queue, std::make_shared<GearmanClient>(se, *it));
            }
        }
#endif
#ifdef WITH_RABBITMQ
        auto rabbitmqConfig = cfg->GetRabbitmqConfiguration();
        for (auto it = rabbitmqConfig->begin(); it != rabbitmqConfig->end(); ++it) {
            auto queues = (*it)->GetQueues();
            for (auto queue = queues->begin(); queue != queues->end(); ++queue) {
                InsertHandler(*queue, std::make_shared<RabbitmqClient>(se, *it));
            }
        }
#endif

        for (auto qHandlerPair = handlers.begin(); qHandlerPair != handlers.end(); ++qHandlerPair) {
            mqHandlers[qHandlerPair->first] = std::make_shared<MessageQueueHandler>(
                se, this, maxBulkSize, &globalBulkCounter, qHandlerPair->first, qHandlerPair->second);
        }
    }

    MessageHandlerList::~MessageHandlerList() {}

    void MessageHandlerList::FlushBulkQueue() {
        for (auto it = mqHandlers.begin(); it != mqHandlers.end(); ++it) {
            it->second->FlushBulkQueue();
        }
        globalBulkCounter = 0;
    }

    bool MessageHandlerList::Connect() {
        for (auto it = mqHandlers.begin(); it != mqHandlers.end(); ++it) {
            if (!it->second->Connect()) {
                return false;
            }
        }
        return true;
    }

} // namespace statusengine
