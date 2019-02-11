#pragma once

#include "Configuration/Queue.h"


namespace statusengine {
    class IMessageHandlerList {
      public:
        virtual ~IMessageHandlerList() = default;

        virtual void InitComplete() = 0;
        virtual bool Connect() = 0;
        virtual void FlushBulkQueue() = 0;
        virtual void Worker() = 0;
        virtual bool QueueExists(Queue queue) = 0;

    };
}
