#pragma once

#include "MessageHandler.h"

#include "libgearman-1.0/gearman.h"

namespace statusengine {
    class GearmanClient : public MessageHandler {
      public:
        explicit GearmanClient(Statusengine *se, const std::string &url);
        ~GearmanClient();

        virtual bool Connect();

        virtual void SendMessage(const std::string &queue, const std::string &message) const;

      private:
        std::string url;
        gearman_client_st *client;
    };
} // namespace statusengine
