#pragma once

#include <memory>
#include <string>
#include <vector>

#include "vendor/toml.hpp"

#include "LogStream.h"
#include "Statusengine.h"

namespace statusengine {
    class RabbitmqConfiguration;

    class Configuration {
      public:
        Configuration(Statusengine *se);
        ~Configuration();
        bool Load(std::string);

        bool GetQueueHostStatus() const;
        bool GetQueueHostCheck() const;
        bool GetQueueOCHP() const;
        bool GetQueueServiceStatus() const;
        bool GetQueueServiceCheck() const;
        bool GetQueueServicePerfData() const;
        bool GetQueueOCSP() const;
        bool GetQueueStateChange() const;
        bool GetQueueLogData() const;
        bool GetQueueSystemCommandData() const;
        bool GetQueueCommentData() const;
        bool GetQueueExternalCommandData() const;
        bool GetQueueAcknowledgementData() const;
        bool GetQueueFlappingData() const;
        bool GetQueueDowntimeData() const;
        bool GetQueueNotificationData() const;
        bool GetQueueProgramStatusData() const;
        bool GetQueueContactStatusData() const;
        bool GetQueueContactNotificationData() const;
        bool GetQueueContactNotificationMethodData() const;
        bool GetQueueEventHandlerData() const;
        bool GetQueueProcessData() const;
        bool GetQueueRestartData() const;

        std::vector<std::string> GetGearmanList();
        std::vector<RabbitmqConfiguration *> GetRabbitmqConfiguration();

        template <typename T> T GetTomlIgnore(const toml::Table &tab, const char *ky, T &&opt) const {
            try {
                return toml::get_or(tab, ky, opt);
            }
            catch (const toml::type_error &tte) {
                se->Log() << "Invalid configuration: Invalid value for key " << ky << eoem;
            }
            return std::move(opt);
        }

      private:
        Statusengine *se;
        toml::Table cfg;
        toml::Table queueTable;
        toml::Table gearmanTable;
        toml::Table amqpTable;

        std::vector<RabbitmqConfiguration *> rabbitmq;
    };
} // namespace statusengine
