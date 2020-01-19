#pragma once

#include "LogStream.h"
#include "MessageHandler/IMessageHandler.h"


namespace statusengine {
    class IStatusengine {
      public:
        virtual ~IStatusengine() = default;

        virtual LogStream &Log() = 0;
        virtual void FlushBulkQueue() = 0;
        virtual IMessageHandlerList *GetMessageHandler() const = 0;
        virtual void InitEventCallbacks() = 0;
        virtual time_t GetStartupScheduleMax() const = 0;
    };
}

