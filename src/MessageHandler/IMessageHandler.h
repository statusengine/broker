#pragma  once

#include <json.h>
#include <memory>
#include <string_view>

#include "../Queue.h"


namespace statusengine {
    class IMessageHandler {
    public:
        virtual ~IMessageHandler() = default;

        virtual void ProcessMessage(WorkerQueue workerQueue, const std::string &message) = 0;
        virtual void ProcessMessage(WorkerQueue workerQueue, json_object *obj) = 0;

        virtual bool Connect() = 0;
        virtual bool Worker(unsigned long &counter) = 0;
        virtual void SendMessage(Queue queue, const std::string_view msg) = 0;
    };

    class IMessageQueueHandler {
    public:
        virtual ~IMessageQueueHandler() = default;

        virtual void SendMessage(std::string s) = 0;
        virtual void FlushBulkQueue() = 0;
    };

    class IMessageHandlerList {
    public:
        virtual ~IMessageHandlerList() = default;

        virtual void InitComplete() = 0;
        virtual bool Connect() = 0;
        virtual void FlushBulkQueue() = 0;
        virtual void Worker() = 0;
        virtual bool QueueExists(Queue queue) = 0;
        virtual std::shared_ptr<IMessageQueueHandler> GetMessageQueueHandler(Queue queue) = 0;
    };
}

