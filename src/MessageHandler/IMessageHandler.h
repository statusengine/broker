#pragma  once

#include <json.h>
#include <memory>

#include "Queue.h"
#include "NagiosObject.h"


namespace statusengine {
    class IMessageHandler {
    public:
        virtual ~IMessageHandler() = default;

        virtual void ProcessMessage(WorkerQueue workerQueue, const std::string &message) = 0;
        virtual void ProcessMessage(WorkerQueue workerQueue, json_object *obj) = 0;

        virtual bool Connect() = 0;
        virtual bool Worker(unsigned long &counter) = 0;
        virtual void SendMessage(Queue queue, const std::string &message) = 0;
    };

    class IMessageQueueHandler {
    public:
        virtual ~IMessageQueueHandler() = default;

        virtual void SendMessage(NagiosObject &obj) = 0;
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

