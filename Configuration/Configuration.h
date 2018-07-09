#pragma once

#include <ctime>
#include <memory>
#include <string>
#include <vector>

#include "vendor/toml.hpp"

#include "Statusengine.h"

namespace statusengine {
    class RabbitmqConfiguration;
    class GearmanConfiguration;

    enum class Queue {
        HostStatus,
        HostCheck,
        ServiceStatus,
        ServiceCheck,
        ServicePerfData,
        StateChange,
        LogData,
        AcknowledgementData,
        FlappingData,
        DowntimeData,
        ContactNotificationMethodData,
        RestartData,
        SystemCommandData,
        CommentData,
        ExternalCommandData,
        NotificationData,
        ProgramStatusData,
        ContactStatusData,
        ContactNotificationData,
        EventHandlerData,
        ProcessData,
        BulkOCSP,
        OCSP,
        BulkOCHP,
        OCHP
    };

    class Configuration {
      public:
        Configuration(Statusengine *se);
        ~Configuration();
        bool Load(std::string);

        time_t GetBulkFlushInterval() const;
        unsigned long GetBulkMaximum() const;
        time_t GetStartupScheduleMax() const;

        std::vector<std::shared_ptr<GearmanConfiguration>> *GetGearmanConfiguration();
        std::vector<std::shared_ptr<RabbitmqConfiguration>> *GetRabbitmqConfiguration();

        static const std::map<std::string, Queue> QueueName;
        static const std::map<Queue, std::string> QueueId;

      private:
        Statusengine *se;
        toml::Table cfg;
        toml::Table bulkTable;
        toml::Table schedulerTable;

        std::vector<std::shared_ptr<RabbitmqConfiguration>> rabbitmq;
        std::vector<std::shared_ptr<GearmanConfiguration>> gearman;

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
