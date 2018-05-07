#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>
#include <vector>

#include "vendor/toml.hpp"

#include "LogStream.h"
#include "Statusengine.h"

namespace statusengine {
    class Configuration {
      public:
        Configuration(Statusengine *se);
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

      private:
        Statusengine *se;
        toml::Table cfg;
        toml::Table queueTable;
        toml::Table gearmanTable;
        toml::Table amqpTable;
    };
} // namespace statusengine

#endif // !CONFIGURATION_H
