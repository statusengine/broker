#pragma once

#include "vendor/toml.hpp"
#include <string>

namespace statusengine {
    class Statusengine;

    class MessageHandler {
      public:
        explicit MessageHandler(Statusengine *se);
        ~MessageHandler();

        virtual bool Connect() = 0;

        virtual void SendMessage(const std::string &queue, const std::string &message) = 0;

      protected:
        Statusengine *se;
    };
} // namespace statusengine
