#pragma once

#include <ctime>
#include <string>

#include "MessageHandlerConfiguration.h"

namespace statusengine {
    class RabbitmqConfiguration : public MessageHandlerConfiguration {
      public:
        RabbitmqConfiguration(Statusengine *se);
        ~RabbitmqConfiguration();
        virtual bool Load(const toml::Table &tbl);
        std::string Hostname;
        int Port;
        timeval *Timeout;
        std::string Vhost;
        std::string Username;
        std::string Password;
        std::string Exchange;
        bool DurableExchange;
        bool DurableQueues;
        bool SSL;
        bool SSLVerify;
        std::string SSLCacert;
        std::string SSLCert;
        std::string SSLKey;
    };
} // namespace statusengine
