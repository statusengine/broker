#include "Configuration.h"

#include "RabbitmqConfiguration.h"
#include "Statusengine.h"

namespace statusengine {

    Configuration::Configuration(Statusengine *se) : se(se) {}

    Configuration::~Configuration() {
        rabbitmq.clear();
    }

    bool Configuration::Load(std::string configurationPath) {
        try {
            cfg = toml::parse(configurationPath);
        }
        catch (std::runtime_error &rte) {
            se->Log() << "Could not read file: " << rte.what() << LogLevel::Error;
            return false;
        }
        catch (toml::syntax_error &ste) {
            se->Log() << "configuration syntax error: " << ste.what() << LogLevel::Error;
            return false;
        }

        try {
            queueTable = cfg.at("Queues").cast<toml::value_t::Table>();
        }
        catch (std::out_of_range &oor) {
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Queues isn't a table!" << LogLevel::Error;
            return false;
        }

        try {
            schedulerTable = cfg.at("Scheduler").cast<toml::value_t::Table>();
        }
        catch (std::out_of_range &oor) {
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Scheduler isn't a table!" << LogLevel::Error;
            return false;
        }

        try {
            std::vector<toml::Table> gearmanTables = toml::get<std::vector<toml::Table>>(cfg.at("Gearman"));
            for (auto it = gearmanTables.begin(); it != gearmanTables.end(); ++it) {
                try {
                    gearmanUrls.push_back((*it).at("URL").cast<toml::value_t::String>());
                }
                catch (std::out_of_range &oor) {
                    se->Log() << "Invalid configuration: The gearman section doesn't contain an URL!" << LogLevel::Error;
                    return false;
                }
                catch (const toml::type_error &tte) {
                    se->Log() << "Invalid configuration: The gearman URL must be a string!" << LogLevel::Error;
                    return false;
                }
            }
        }
        catch (const std::out_of_range &oor) {
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Gearman isn't an Array of Tables!" << LogLevel::Error;
            return false;
        }

        try {
            std::vector<toml::Table> rabbits = toml::get<std::vector<toml::Table>>(cfg.at("Rabbitmq"));
            for (auto it = rabbits.begin(); it != rabbits.end(); ++it) {
                auto rfg = new RabbitmqConfiguration(se, this);
                if (!rfg->Load(*it)) {
                    return false;
                }
                rabbitmq.push_back(rfg);
            }
        }
        catch (const std::out_of_range &oor) {
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Rabbitmq isn't an Array of Tables!" << LogLevel::Error;
            return false;
        }

        return true;
    }

    bool Configuration::GetQueueHostStatus() const {
        return GetTomlIgnore<>(queueTable, "HostStatus", false);
    }

    bool Configuration::GetQueueHostCheck() const {
        return GetTomlIgnore<>(queueTable, "HostCheck", false);
    }

    bool Configuration::GetQueueOCHP() const {
        return GetTomlIgnore<>(queueTable, "OCHP", false);
    }

    bool Configuration::GetQueueServiceStatus() const {
        return GetTomlIgnore<>(queueTable, "ServiceStatus", false);
    }

    bool Configuration::GetQueueServiceCheck() const {
        return GetTomlIgnore<>(queueTable, "ServiceCheck", false);
    }

    bool Configuration::GetQueueServicePerfData() const {
        return GetTomlIgnore<>(queueTable, "ServicePerfData", false);
    }

    bool Configuration::GetQueueOCSP() const {
        return GetTomlIgnore<>(queueTable, "OCSP", false);
    }

    bool Configuration::GetQueueStateChange() const {
        return GetTomlIgnore<>(queueTable, "StateChange", false);
    }

    bool Configuration::GetQueueLogData() const {
        return GetTomlIgnore<>(queueTable, "LogData", false);
    }

    bool Configuration::GetQueueSystemCommandData() const {
        return GetTomlIgnore<>(queueTable, "SystemCommandData", false);
    }

    bool Configuration::GetQueueCommentData() const {
        return GetTomlIgnore<>(queueTable, "CommentData", false);
    }

    bool Configuration::GetQueueExternalCommandData() const {
        return GetTomlIgnore<>(queueTable, "ExternalCommandData", false);
    }

    bool Configuration::GetQueueAcknowledgementData() const {
        return GetTomlIgnore<>(queueTable, "AcknowledgementData", false);
    }

    bool Configuration::GetQueueFlappingData() const {
        return GetTomlIgnore<>(queueTable, "FlappingData", false);
    }

    bool Configuration::GetQueueDowntimeData() const {
        return GetTomlIgnore<>(queueTable, "DowntimeData", false);
    }

    bool Configuration::GetQueueNotificationData() const {
        return GetTomlIgnore<>(queueTable, "NotificationData", false);
    }

    bool Configuration::GetQueueProgramStatusData() const {
        return GetTomlIgnore<>(queueTable, "ProgramStatusData", false);
    }

    bool Configuration::GetQueueContactStatusData() const {
        return GetTomlIgnore<>(queueTable, "ContactStatusData", false);
    }

    bool Configuration::GetQueueContactNotificationData() const {
        return GetTomlIgnore<>(queueTable, "ContactNotificationData", false);
    }

    bool Configuration::GetQueueContactNotificationMethodData() const {
        return GetTomlIgnore<>(queueTable, "ContactNotificationMethodData", false);
    }

    bool Configuration::GetQueueEventHandlerData() const {
        return GetTomlIgnore<>(queueTable, "EventHandlerData", false);
    }

    bool Configuration::GetQueueProcessData() const {
        return GetTomlIgnore<>(queueTable, "ProcessData", false);
    }

    bool Configuration::GetQueueRestartData() const {
        return GetTomlIgnore<>(queueTable, "RestartData", false);
    }

    time_t Configuration::GetStartupScheduleMax() const {
        return GetTomlIgnore<>(schedulerTable, "StartupScheduleMax", 0);
    }

    std::vector<std::string> Configuration::GetGearmanList() {
        return gearmanUrls;
    }

    std::vector<RabbitmqConfiguration *> Configuration::GetRabbitmqConfiguration() {
        return rabbitmq;
    }
} // namespace statusengine
