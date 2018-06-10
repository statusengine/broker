#pragma once

#include <map>
#include <string>
#include <vector>

namespace statusengine {
    class Statusengine;

    class MessageHandler {
      public:
        explicit MessageHandler(Statusengine *se);
        ~MessageHandler();

        virtual bool Connect() = 0;

        virtual void SendMessage(const std::string &queue, const std::string &message) const = 0;

      protected:
        Statusengine *se;
    };
} // namespace statusengine
