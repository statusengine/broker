#pragma once

#include "MessageHandler.h"

#include "libgearman-1.0/gearman.h"

namespace statusengine {
    class GearmanClient : public MessageHandler {
      public:
        explicit GearmanClient(Statusengine *se, const std::string &url);
        ~GearmanClient();

        void SendMessage(const std::string &queue, const std::string &message) const;

      private:
        Statusengine *se;
        gearman_client_st *client;
    };
} // namespace statusengine
