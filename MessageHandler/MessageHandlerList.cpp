#include "MessageHandlerList.h"

#include "Configuration.h"
#include "MessageQueueHandler.h"


#ifdef WITH_GEARMAN
#include "GearmanClient.h"
#endif

#ifdef WITH_RABBITMQ
#include "RabbitmqClient.h"
#endif

namespace statusengine {
    MessageHandlerList::MessageHandlerList(IStatusengine *se, Configuration *cfg)
        : se(se), maxBulkSize(0), globalBulkCounter(0), flushInProgress(true) {
        // flushInProgress is set to true to ensure no messages are sent until initialization is complete

        maxBulkSize = cfg->GetBulkMaximum();
        std::map<Queue, std::shared_ptr<std::vector<std::shared_ptr<IMessageHandler>>>> handlers;
        auto InsertHandler = [&handlers](Queue queue, std::shared_ptr<IMessageHandler> handler) {
            std::shared_ptr<std::vector<std::shared_ptr<IMessageHandler>>> queueHandlers;
            try {
                queueHandlers = handlers.at(queue);
            }
            catch (std::out_of_range &oor) {
                queueHandlers = std::make_shared<std::vector<std::shared_ptr<IMessageHandler>>>();
                handlers[queue] = queueHandlers;
            }
            queueHandlers->push_back(handler);
        };
        maxWorkerMessagesPerInterval = cfg->GetMaxWorkerMessagesPerInterval();

#ifdef WITH_GEARMAN
        auto gearmanConfigs = cfg->GetGearmanConfiguration();
        for (auto &gearmanConfig : *gearmanConfigs) {
            auto gearmanClient = std::make_shared<GearmanClient>(se, gearmanConfig);
            allHandlers.push_back(gearmanClient);
            auto queues = gearmanConfig->GetQueues();
            for (auto &queue : *queues) {
                InsertHandler(queue, gearmanClient);
            }
        }
#endif
#ifdef WITH_RABBITMQ
        auto rabbitmqConfigs = cfg->GetRabbitmqConfiguration();
        for (auto &rabbitmqConfig : *rabbitmqConfigs) {
            auto rabbitmqClient = std::make_shared<RabbitmqClient>(se, rabbitmqConfig);
            allHandlers.push_back(rabbitmqClient);
            auto queues = rabbitmqConfig->GetQueues();
            for (auto &queue : *queues) {
                InsertHandler(queue, rabbitmqClient);
            }
        }
#endif
        for (auto &qHandlerPair : handlers) {
            mqHandlers[qHandlerPair.first] = std::make_shared<MessageQueueHandler>(
                *se, *this, maxBulkSize, &globalBulkCounter, qHandlerPair.first, qHandlerPair.second, cfg->IsBulkQueue(qHandlerPair.first));
        }
    }

    MessageHandlerList::~MessageHandlerList() {
        FlushBulkQueue();
    }

    void MessageHandlerList::InitComplete() {
        flushInProgress = false;
    }

    void MessageHandlerList::FlushBulkQueue() {
        if (globalBulkCounter > 0 && !flushInProgress) {
            flushInProgress = true;
            se->Log() << "Flush Bulk Queues" << LogLevel::Info;

            for (auto &handler : mqHandlers) {
                handler.second->FlushBulkQueue();
            }
            globalBulkCounter = 0;
            flushInProgress = false;
        }
    }

    bool MessageHandlerList::Connect() {
        for (auto &handler : allHandlers) {
            if (!handler->Connect()) {
                return false;
            }
        }
        return true;
    }

    std::shared_ptr<IMessageQueueHandler> MessageHandlerList::GetMessageQueueHandler(Queue queue) {
        return mqHandlers.at(queue);
    }

    bool MessageHandlerList::QueueExists(Queue queue) {
        return mqHandlers.find(queue) != mqHandlers.end();
    }

    void MessageHandlerList::Worker() {
        unsigned long counter = 0ul;
        bool moreMessages;
        do {
            moreMessages = false;
            for (auto &handler : allHandlers) {
                if (handler->Worker(counter)) {
                    moreMessages = true;
                }
            }
        } while (moreMessages && (counter < maxWorkerMessagesPerInterval));
    }

} // namespace statusengine
