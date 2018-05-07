#include "Configuration.h"

#include <list>

#include "Statusengine.h"

namespace statusengine {

    Configuration::Configuration(Statusengine *se) : se(se) {}

    bool Configuration::Load(std::string configurationPath) {
        try {
            cfg = toml::parse(configurationPath);
        }
        catch (std::runtime_error &rte) {
            se->Log() << "Could not read file: " << rte.what() << eoem;
            return false;
        }
        catch (toml::syntax_error &ste) {
            se->Log() << "configuration syntax error: " << ste.what() << eoem;
            return false;
        }

        try {
            queueTable = cfg.at("Queues").cast<toml::value_t::Table>();
        }
        catch (std::out_of_range &oor) {
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Queues isn't a table!" << eoem;
            return false;
        }

        try {
            gearmanTable = cfg.at("Gearman").cast<toml::value_t::Table>();
        }
        catch (const std::out_of_range &oor) {
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Gearman isn't a table!" << eoem;
            return false;
        }

        return true;
    }

    bool Configuration::GetQueueHostStatus() const {
        return GetTomlIgnore(queueTable, "HostStatus", false);
    }

    bool Configuration::GetQueueHostCheck() const {
        return GetTomlIgnore(queueTable, "HostCheck", false);
    }

    bool Configuration::GetQueueOCHP() const {
        return GetTomlIgnore(queueTable, "OCHP", false);
    }

    bool Configuration::GetQueueServiceStatus() const {
        return GetTomlIgnore(queueTable, "ServiceStatus", false);
    }

    bool Configuration::GetQueueServiceCheck() const {
        return GetTomlIgnore(queueTable, "ServiceCheck", false);
    }

    bool Configuration::GetQueueServicePerfData() const {
        return GetTomlIgnore(queueTable, "ServicePerfData", false);
    }

    bool Configuration::GetQueueOCSP() const {
        return GetTomlIgnore(queueTable, "OCSP", false);
    }

    bool Configuration::GetQueueStateChange() const {
        return GetTomlIgnore(queueTable, "StateChange", false);
    }

    bool Configuration::GetQueueLogData() const {
        return GetTomlIgnore(queueTable, "LogData", false);
    }

    bool Configuration::GetQueueSystemCommandData() const {
        return GetTomlIgnore(queueTable, "SystemCommandData", false);
    }

    bool Configuration::GetQueueCommentData() const {
        return GetTomlIgnore(queueTable, "CommentData", false);
    }

    bool Configuration::GetQueueExternalCommandData() const {
        return GetTomlIgnore(queueTable, "ExternalCommandData", false);
    }

    bool Configuration::GetQueueAcknowledgementData() const {
        return GetTomlIgnore(queueTable, "AcknowledgementData", false);
    }

    bool Configuration::GetQueueFlappingData() const {
        return GetTomlIgnore(queueTable, "FlappingData", false);
    }

    bool Configuration::GetQueueDowntimeData() const {
        return GetTomlIgnore(queueTable, "DowntimeData", false);
    }

    bool Configuration::GetQueueNotificationData() const {
        return GetTomlIgnore(queueTable, "NotificationData", false);
    }

    bool Configuration::GetQueueProgramStatusData() const {
        return GetTomlIgnore(queueTable, "ProgramStatusData", false);
    }

    bool Configuration::GetQueueContactStatusData() const {
        return GetTomlIgnore(queueTable, "ContactStatusData", false);
    }

    bool Configuration::GetQueueContactNotificationData() const {
        return GetTomlIgnore(queueTable, "ContactNotificationData", false);
    }

    bool Configuration::GetQueueContactNotificationMethodData() const {
        return GetTomlIgnore(queueTable, "ContactNotificationMethodData", false);
    }

    bool Configuration::GetQueueEventHandlerData() const {
        return GetTomlIgnore(queueTable, "EventHandlerData", false);
    }

    bool Configuration::GetQueueProcessData() const {
        return GetTomlIgnore(queueTable, "ProcessData", false);
    }

    bool Configuration::GetQueueRestartData() const {
        return GetTomlIgnore(queueTable, "RestartData", false);
    }

    std::vector<std::string> Configuration::GetGearmanList() {
        try {
            return toml::get<std::vector<std::string>>(gearmanTable.at("Servers"));
        }
        catch (std::out_of_range &oor) {
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Gearman::Servers isn't an array! "
                         "Ignoring for now..."
                      << eoem;
        }
        std::vector<std::string> emptyResult;
        return emptyResult;
    }

    template <typename T> T GetTomlIgnore(const toml::Table &tab, const toml::key &ky, T &&opt) {
        try {
            return toml::get_or(tab, ky, opt);
        }
        catch (const toml::type_error &tte) {
            se->Log() << "Invalid configuration: Invalid value for key " << ky << eoem;
        }
        return std::move(opt);
    }

} // namespace statusengine
