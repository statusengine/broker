#pragma once

#include "LogStream.h"
#include "MessageHandler/IMessageHandlerList.h"


namespace statusengine {
    class IStatusengine {
      public:
        virtual LogStream &Log() = 0;
        virtual void FlushBulkQueue() = 0;
        virtual IMessageHandlerList *GetMessageHandler() const = 0;
        virtual void InitEventCallbacks() = 0;

    };
}

