#pragma once

#include "NagiosObject.h"

namespace statusengine {
    class IMessageQueueHandler {
    public:
        virtual void SendMessage(NagiosObject &obj) = 0;
        virtual void FlushBulkQueue() = 0;
    };
}
