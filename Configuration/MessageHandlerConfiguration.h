#pragma once

#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "vendor/toml.hpp"

#include "Configuration.h"
#include "Statusengine.h"

namespace statusengine {
    class MessageHandlerConfiguration {
      public:
        bool InitLoad(const toml::Table &tbl);
        virtual bool Load(const toml::Table &tbl) = 0;

        virtual const std::shared_ptr<std::map<Queue, std::string>> GetQueueNames() const;
        virtual std::shared_ptr<std::set<Queue>> GetQueues() const;

      protected:
        MessageHandlerConfiguration(Statusengine *se);

        Statusengine *se;

        std::shared_ptr<std::map<Queue, std::string>> queues;

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