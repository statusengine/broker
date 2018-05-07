#pragma once

#include <string>

namespace statusengine {
    class Statusengine;

    class MessageHandler {
      public:
        explicit MessageHandler(Statusengine *se);
        ~MessageHandler();

        virtual void SendMessage(const std::string &queue, const std::string &message) const = 0;

      protected:
        Statusengine *se;
    };
} // namespace statusengine
