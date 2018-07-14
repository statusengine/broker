#pragma once

#include <ctime>
#include <string>

#include "MessageHandlerConfiguration.h"
#include "Statusengine.h"

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

      private:
        template <typename T> T GetTomlDefault(const toml::Table &tab, const char *ky, T &&opt) const {
            try {
                return toml::get_or(tab, ky, opt);
            }
            catch (const toml::type_error &tte) {
                se->Log() << "Invalid configuration: Invalid value for key " << ky << LogLevel::Error;
            }
            return std::move(opt);
        }
    };
} // namespace statusengine
