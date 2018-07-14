#pragma once

#include "vendor/toml.hpp"
#include <memory>
#include <set>
#include <string>

#include "Configuration/MessageHandlerConfiguration.h"
#include "Configuration/Queue.h"

namespace statusengine {
    class Statusengine;

    class MessageHandler {
      public:
        explicit MessageHandler(Statusengine *se);
        ~MessageHandler();

        virtual bool Connect() = 0;
        virtual void SendMessage(Queue queue, const std::string &message) = 0;

      protected:
        Statusengine *se;
    };
} // namespace statusengine
