#include "RabbitmqConfiguration.h"

#include "Statusengine.h"

namespace statusengine {
    RabbitmqConfiguration::RabbitmqConfiguration(Statusengine *se)
        : MessageHandlerConfiguration(se), Port(5673), Timeout(nullptr), SSL(false), SSLVerify(true),
          Exchange("statusengine") {}

    RabbitmqConfiguration::~RabbitmqConfiguration() {
        delete Timeout;
    }

    bool RabbitmqConfiguration::Load(const toml::Table &tbl) {
        Hostname = GetTomlDefault<>(tbl, "Hostname", std::string(""));

        if (Hostname.empty()) {
            se->Log() << "Please specify a hostname in rabbitmq configuration" << LogLevel::Error;
            return false;
        }

        Port = GetTomlDefault<>(tbl, "Port", 5672);

        Vhost = GetTomlDefault<>(tbl, "Vhost", std::string("/"));
        Username = GetTomlDefault<>(tbl, "Username", std::string("statusengine"));
        Password = GetTomlDefault<>(tbl, "Password", std::string(""));
        if (Password.empty()) {
            se->Log() << "Warning, no password specified" << LogLevel::Warning;
        }

        int tov = GetTomlDefault<>(tbl, "Timeout", 30);
        Timeout = new timeval;
        Timeout->tv_sec = tov;
        Timeout->tv_usec = 0;

        Exchange = GetTomlDefault<>(tbl, "Exchange", std::string("statusengine"));

        DurableExchange = GetTomlDefault<>(tbl, "DurableExchange", false);
        DurableQueues = GetTomlDefault<>(tbl, "DurableQueues", false);

        SSL = GetTomlDefault<>(tbl, "SSL", false);

        SSLVerify = GetTomlDefault<>(tbl, "SSL_verify", true);
        SSLCacert = GetTomlDefault<>(tbl, "SSL_cacert", std::string(""));
        SSLCert = GetTomlDefault<>(tbl, "SSL_cert", std::string(""));
        SSLKey = GetTomlDefault<>(tbl, "SSL_key", std::string(""));

        return true;
    }
} // namespace statusengine
