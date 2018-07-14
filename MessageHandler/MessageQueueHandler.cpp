#include "MessageQueueHandler.h"

#include "Configuration/Configuration.h"
#include "MessageHandlerList.h"
#include "NagiosObjects/NagiosObject.h"

namespace statusengine {
    MessageQueueHandler::MessageQueueHandler(Statusengine *se, MessageHandlerList *mhlist, unsigned long maxBulkSize,
                                             unsigned long *globalBulkCounter, Queue queue,
                                             std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> handlers)
        : se(se), mhlist(mhlist), maxBulkSize(maxBulkSize), globalBulkCounter(globalBulkCounter), queue(queue),
          handlers(handlers) {}

    void MessageQueueHandler::SendMessage(const std::string &message) {
        for (auto &handler : *handlers) {
            handler->SendMessage(queue, message);
        }
    }

    void MessageQueueHandler::SendBulkMessage(std::string message) {
        bulkMessages.push_back(message);
        if (++(*globalBulkCounter) >= maxBulkSize) {
            mhlist->FlushBulkQueue();
        }
    }

    void MessageQueueHandler::FlushBulkQueue() {
        if (bulkMessages.size() > 0) {
            NagiosObject obj;
            json_object *arr = json_object_new_array();
            for (auto &message : bulkMessages) {
                json_object_array_add(arr, json_object_new_string(message.c_str()));
            }
            obj.SetData<>("Messages", arr);
            obj.SetData<>("Compression", "plain");
            se->Log() << "Send bulk message (" << bulkMessages.size() << ") for queue "
                      << Configuration::QueueId.at(queue) << LogLevel::Info;
            SendMessage(obj.ToString());
            bulkMessages.clear();
        }
    }

    bool MessageQueueHandler::Connect() {
        for (auto it = handlers->begin(); it != handlers->end(); ++it) {
            if (!(*it)->Connect()) {
                return false;
            }
        }
        return true;
    }
} // namespace statusengine
