#include "RabbitmqConfiguration.h"

#include "Configuration.h"
#include "Statusengine.h"

namespace statusengine {
    RabbitmqConfiguration::RabbitmqConfiguration(Statusengine *se, Configuration *cfg)
        : port(5673), timeout(nullptr), ssl(false), ssl_verify_hostname(true), ssl_verify_peer(true),
          exchange("statusengine"), se(se), cfg(cfg) {}

    RabbitmqConfiguration::~RabbitmqConfiguration() {
        delete timeout;
    }

    bool RabbitmqConfiguration::Load(toml::Table &tbl) {
        hostname = cfg->GetTomlIgnore<>(tbl, "Hostname", std::string(""));

        if (hostname.empty()) {
            se->Log() << "Please specify a hostname in rabbitmq configuration" << eoem;
            return false;
        }

        port = cfg->GetTomlIgnore<>(tbl, "Port", 5672);

        vhost = cfg->GetTomlIgnore<>(tbl, "Vhost", std::string("/"));
        username = cfg->GetTomlIgnore<>(tbl, "Username", std::string("statusengine"));
        password = cfg->GetTomlIgnore<>(tbl, "Password", std::string(""));
        if (password.empty()) {
            se->Log() << "Warning, no password specified" << eowm;
        }

        int tov = cfg->GetTomlIgnore<>(tbl, "Timeout", 30);
        timeout = new timeval;
        timeout->tv_sec = tov;
        timeout->tv_usec = 0;

        exchange = cfg->GetTomlIgnore<>(tbl, "Exchange", std::string("statusengine"));

        durable_exchange = cfg->GetTomlIgnore<>(tbl, "DurableExchange", false);

        ssl = cfg->GetTomlIgnore<>(tbl, "SSL", false);

        ssl_verify_peer = cfg->GetTomlIgnore<>(tbl, "SSL_verify_peer", true);
        ssl_verify_hostname = cfg->GetTomlIgnore<>(tbl, "SSL_verify_hostname", true);
        ssl_cacert = cfg->GetTomlIgnore<>(tbl, "SSL_cacert", std::string(""));
        ssl_cert = cfg->GetTomlIgnore<>(tbl, "SSL_cert", std::string(""));
        ssl_key = cfg->GetTomlIgnore<>(tbl, "SSL_key", std::string(""));

        return true;
    }
} // namespace statusengine
