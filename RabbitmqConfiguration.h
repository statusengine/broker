#pragma once

#include <string>
#include <sys/time.h>

#include "vendor/toml.hpp"

namespace statusengine {
    class Configuration;
    class Statusengine;

    class RabbitmqConfiguration {
      public:
        RabbitmqConfiguration(Statusengine *se, Configuration *cfg);
        ~RabbitmqConfiguration();
        bool Load(toml::Table &tbl);
        std::string hostname;
        int port;
        timeval *timeout;
        std::string vhost;
        std::string username;
        std::string password;
        std::string exchange;
        bool durable_exchange;
        bool ssl;
        bool ssl_verify_peer;
        bool ssl_verify_hostname;
        std::string ssl_cacert;
        std::string ssl_cert;
        std::string ssl_key;

      private:
        Statusengine *se;
        Configuration *cfg;
    };
} // namespace statusengine
