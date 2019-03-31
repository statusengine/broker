#include "MessageQueueHandler.h"

#include "Configuration/Configuration.h"
#include "MessageHandlerList.h"
#include "NagiosObject.h"
#include "Utility.h"

namespace statusengine {
    MessageQueueHandler::MessageQueueHandler(IStatusengine *se, MessageHandlerList *mhlist, unsigned long maxBulkSize,
                                             unsigned long *globalBulkCounter, Queue queue,
                                             std::shared_ptr<std::vector<std::shared_ptr<MessageHandler>>> handlers,
                                             bool bulk)
        : se(se), mhlist(mhlist), maxBulkSize(maxBulkSize), globalBulkCounter(globalBulkCounter), queue(queue),
          handlers(std::move(handlers)), bulk(bulk) {}

    void MessageQueueHandler::SendMessage(NagiosObject &obj) {
        if (bulk) {
            bulkMessages.push_back(new NagiosObject(&obj));
            if (++(*globalBulkCounter) >= maxBulkSize) {
                mhlist->FlushBulkQueue();
            }
        }
        else {
            std::string msg = obj.ToString();
            for (auto &handler : *handlers) {
                handler->SendMessage(queue, msg);
            }
        }
    }

    void MessageQueueHandler::FlushBulkQueue() {
        if (!bulkMessages.empty()) {
            NagiosObject msgObj;
            json_object *arr = json_object_new_array();

            for (auto &obj : bulkMessages) {
                json_object_array_add(arr, obj->GetDataCopy());
            }

            msgObj.SetData("messages", arr);
            msgObj.SetData("format", "none");

            std::string msg = msgObj.ToString();
            for (auto &handler : *handlers) {
                handler->SendMessage(queue, msg);
            }

            se->Log() << "Sent bulk message (" << bulkMessages.size() << ") for queue "
                      << Configuration::QueueId.at(queue) << LogLevel::Info;

            clearContainer<>(&bulkMessages);
        }
    }
} // namespace statusengine
