#include "Configuration.h"

#include "GearmanConfiguration.h"
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
            bulkTable = cfg.at("Bulk").cast<toml::value_t::Table>();
        }
        catch (std::out_of_range &oor) {
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Bulk isn't a table!" << LogLevel::Error;
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
            std::vector<toml::Table> gearmans = toml::get<std::vector<toml::Table>>(cfg.at("Gearman"));
            for (auto it = gearmans.begin(); it != gearmans.end(); ++it) {
                auto gfg = std::make_shared<GearmanConfiguration>(se);
                if (!gfg->Load(*it)) {
                    return false;
                }
                gearman.push_back(gfg);
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
                auto rfg = std::make_shared<RabbitmqConfiguration>(se);
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

    time_t Configuration::GetBulkFlushInterval() const {
        return GetTomlDefault<>(bulkTable, "FlushInterval", static_cast<time_t>(10));
    }

    unsigned long Configuration::GetBulkMaximum() const {
        return GetTomlDefault<>(bulkTable, "Maximum", 200ul);
    }

    time_t Configuration::GetStartupScheduleMax() const {
        return GetTomlDefault<>(schedulerTable, "StartupScheduleMax", 0);
    }

    std::vector<std::shared_ptr<GearmanConfiguration>> *Configuration::GetGearmanConfiguration() {
        return &gearman;
    }

    std::vector<std::shared_ptr<RabbitmqConfiguration>> *Configuration::GetRabbitmqConfiguration() {
        return &rabbitmq;
    }

    const std::map<std::string, Queue> Configuration::QueueName = {
        {"HostStatus", Queue::HostStatus},
        {"HostCheck", Queue::HostCheck},
        {"ServiceStatus", Queue::ServiceStatus},
        {"ServiceCheck", Queue::ServiceCheck},
        {"ServicePerfData", Queue::ServicePerfData},
        {"StateChange", Queue::StateChange},
        {"LogData", Queue::LogData},
        {"AcknowledgementData", Queue::AcknowledgementData},
        {"FlappingData", Queue::FlappingData},
        {"DowntimeData", Queue::DowntimeData},
        {"ContactNotificationMethodData", Queue::ContactNotificationMethodData},
        {"RestartData", Queue::RestartData},
        {"SystemCommandData", Queue::SystemCommandData},
        {"CommentData", Queue::CommentData},
        {"ExternalCommandData", Queue::ExternalCommandData},
        {"NotificationData", Queue::NotificationData},
        {"ProgramStatusData", Queue::ProgramStatusData},
        {"ContactStatusData", Queue::ContactStatusData},
        {"ContactNotificationData", Queue::ContactNotificationData},
        {"EventHandlerData", Queue::EventHandlerData},
        {"ProcessData", Queue::ProcessData},
        {"BulkOCSP", Queue::BulkOCSP},
        {"OCSP", Queue::OCSP},
        {"BulkOCHP", Queue::BulkOCHP},
        {"OCHP", Queue::OCHP}};

    const std::map<Queue, std::string> Configuration::QueueId = {
        {Queue::HostStatus, "HostStatus"},
        {Queue::HostCheck, "HostCheck"},
        {Queue::ServiceStatus, "ServiceStatus"},
        {Queue::ServiceCheck, "ServiceCheck"},
        {Queue::ServicePerfData, "ServicePerfData"},
        {Queue::StateChange, "StateChange"},
        {Queue::LogData, "LogData"},
        {Queue::AcknowledgementData, "AcknowledgementData"},
        {Queue::FlappingData, "FlappingData"},
        {Queue::DowntimeData, "DowntimeData"},
        {Queue::ContactNotificationMethodData, "ContactNotificationMethodData"},
        {Queue::RestartData, "RestartData"},
        {Queue::SystemCommandData, "SystemCommandData"},
        {Queue::CommentData, "CommentData"},
        {Queue::ExternalCommandData, "ExternalCommandData"},
        {Queue::NotificationData, "NotificationData"},
        {Queue::ProgramStatusData, "ProgramStatusData"},
        {Queue::ContactStatusData, "ContactStatusData"},
        {Queue::ContactNotificationData, "ContactNotificationData"},
        {Queue::EventHandlerData, "EventHandlerData"},
        {Queue::ProcessData, "ProcessData"},
        {Queue::BulkOCSP, "BulkOCSP"},
        {Queue::OCSP, "OCSP"},
        {Queue::BulkOCHP, "BulkOCHP"},
        {Queue::OCHP, "OCHP"}};
} // namespace statusengine
