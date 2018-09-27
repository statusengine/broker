#include "MessageQueueHandler.h"

#include "Configuration/Configuration.h"
#include "MessageHandlerList.h"
#include "NagiosObjects/NagiosObject.h"

namespace statusengine {
    MessageQueueHandler::MessageQueueHandler(Statusengine *se, MessageHandlerList *mhlist, unsigned long maxBulkSize,
                                             unsigned long *globalBulkCounter, Queue queue,
                                             std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> handlers,
                                             bool bulk)
        : se(se), mhlist(mhlist), maxBulkSize(maxBulkSize), globalBulkCounter(globalBulkCounter), queue(queue),
          handlers(handlers) {}

    void MessageQueueHandler::SendMessage(NagiosObject *obj) {
        if (bulk) {
            bulkMessages.push_back(obj);
            if (++(*globalBulkCounter) >= maxBulkSize) {
                mhlist->FlushBulkQueue();
            }
        }
        else {
            std::string msg = obj->ToString();
            for (auto &handler : *handlers) {
                handler->SendMessage(queue, msg);
            }
            delete obj;
        }
    }

    void MessageQueueHandler::FlushBulkQueue() {
        if (bulkMessages.size() > 0) {
            NagiosObject *msgObj = new NagiosObject();
            json_object *arr = json_object_new_array();

            for (auto &obj : bulkMessages) {
                json_object_array_add(arr, obj->data);
            }

            msgObj->SetData<>("messages", arr);
            msgObj->SetData<>("format", "none");

            std::string msg = msgObj->ToString();
            for (auto &handler : *handlers) {
                handler->SendMessage(queue, msg);
            }

            se->Log() << "Sent bulk message (" << bulkMessages.size() << ") for queue "
                      << Configuration::QueueId.at(queue) << LogLevel::Info;

            delete msgObj;
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
