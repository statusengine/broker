#pragma once

#include <memory>
#include <vector>

#include "Queue.h"
#include "NagiosObject.h"
#include "IMessageHandler.h"
#include "IStatusengine.h"
#include "Configuration.h"
#include "Utility.h"


namespace statusengine  {

    class MessageQueueHandler : public IMessageQueueHandler {
      public:
        explicit MessageQueueHandler(IStatusengine &se, IMessageHandlerList &mhlist, unsigned long maxBulkSize,
                                                 unsigned long *globalBulkCounter, Queue queue,
                                                 std::shared_ptr<std::vector<std::shared_ptr<IMessageHandler>>> handlers,
                                                 bool bulk)
                : se(se), mhlist(mhlist), maxBulkSize(maxBulkSize), globalBulkCounter(globalBulkCounter), queue(queue),
                  handlers(std::move(handlers)), bulk(bulk) {}

        /**
        * SendMessage
        * @param JsonObjectContainer contains json object that will be deleted after sending the message
        */
        void SendMessage(NagiosObject &obj) override {
            if (bulk) {
                bulkMessages.push_back(new NagiosObject(&obj));
                if (++(*globalBulkCounter) >= maxBulkSize) {
                    mhlist.FlushBulkQueue();
                }
            }
            else {
                std::string msg = obj.ToString();
                for (auto &handler : *handlers) {
                    handler->SendMessage(queue, msg);
                }
            }
        }

        void FlushBulkQueue() override {
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

                auto QueueId = QueueNameHandler::Instance().QueueIds();
                se.Log() << "Sent bulk message (" << bulkMessages.size() << ") for queue "
                         << QueueId.at(queue) << LogLevel::Info;

                clearContainer<>(&bulkMessages);
            }
        }

      private:
        IMessageHandlerList &mhlist;
        IStatusengine &se;

        Queue queue;
        std::shared_ptr<std::vector<std::shared_ptr<IMessageHandler>>> handlers;
        std::vector<NagiosObject *> bulkMessages;

        unsigned long maxBulkSize;
        unsigned long *globalBulkCounter;
        bool bulk;
    };
} // namespace statusengine
