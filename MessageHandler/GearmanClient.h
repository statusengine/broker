#pragma once

#include "MessageHandler.h"

#include "libgearman-1.0/gearman.h"

namespace statusengine {
    class GearmanConfiguration;

    class GearmanClient : public MessageHandler {
      public:
        explicit GearmanClient(Statusengine *se, std::shared_ptr<GearmanConfiguration> cfg);
        ~GearmanClient();

        virtual bool Connect();

        virtual void SendMessage(Queue queue, const std::string &message);

      private:
        gearman_client_st *client;

        std::shared_ptr<GearmanConfiguration> cfg;
        std::shared_ptr<std::map<Queue, std::string>> queueNames;
    };
} // namespace statusengine
