#pragma once

#include <memory>
#include <vector>

#include "MessageHandler.h"

namespace statusengine {
    class Statusengine;
    class Configuration;

    class MessageHandlerList {
      public:
        MessageHandlerList(Statusengine *se, Configuration *cfg);
        ~MessageHandlerList();

        virtual bool Connect();

        virtual void SendMessage(const std::string &queue, const std::string &message);
        virtual void SendBulkMessage(std::string queue, std::string message);
        virtual void FlushBulkQueue();

      private:
        Statusengine *se;
        std::vector<std::shared_ptr<MessageHandler>> handlers;
        std::map<std::string, std::vector<std::string> *> bulkMessages;
        unsigned long maxBulkSize;
        unsigned long globalBulkCounter;
    };
} // namespace statusengine
