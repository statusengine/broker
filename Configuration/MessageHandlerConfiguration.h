#pragma once

#include "Statusengine.h"

#include <string>
#include <unordered_map>

#include "vendor/toml.hpp"

namespace statusengine {
    enum class Queue;

    class MessageHandlerConfiguration {
      public:
        bool InitLoad(const toml::Table &tbl);
        virtual bool Load(const toml::Table &tbl) = 0;

        static const std::unordered_map<Queue, std::string> QueueName;

        virtual const std::unordered_map<std::string, std::string> &GetQueues() const;

      protected:
        MessageHandlerConfiguration(Statusengine *se);

        Statusengine *se;

        std::unordered_map<std::string, std::string> queues;

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