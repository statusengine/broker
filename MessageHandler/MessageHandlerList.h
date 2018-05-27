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

      private:
        Statusengine *se;
        std::vector<std::shared_ptr<MessageHandler>> handlers;
    };
} // namespace statusengine
