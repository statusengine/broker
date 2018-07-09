#include "MessageHandlerConfiguration.h"

#include <string>

namespace statusengine {
    MessageHandlerConfiguration::MessageHandlerConfiguration(Statusengine *se) : se(se) {}

    bool MessageHandlerConfiguration::InitLoad(const toml::Table &tbl) {
        for (auto it = tbl.begin(); it != tbl.end(); ++it) {
            if (std::find(QueueTypes, std::begin(QueueTypes), std::end(QueueTypes)) != std::end(QueueTypes)) {
                try {
                    queues[it->first] = toml::get<std::string>(it->second);
                }
                catch (const toml::type_error &tte) {
                    se->Log() << "Invalid configuration: Invalid value for key " << it->first << LogLevel::Error;
                    return false;
                }
            }
        }
        return Load(tbl);
    }

    const std::unordered_map<std::string, std::string> &MessageHandlerConfiguration::GetQueues() const {
        return queues;
    }

    const std::unordered_map<Queue, std::string> MessageHandlerConfiguration::QueueName = {
        {Queue::HostStatus, "HostStatus"},
        {Queue::HostCheck, "HostCheck"},
        {Queue::HostCheck, "ServiceStatus"},
        {Queue::HostCheck, "ServiceCheck"},
        {Queue::HostCheck, "ServicePerfData"},
        {Queue::HostCheck, "StateChange"},
        {Queue::HostCheck, "LogData"},
        {Queue::HostCheck, "AcknowledgementData"},
        {Queue::HostCheck, "FlappingData"},
        {Queue::HostCheck, "DowntimeData"},
        {Queue::HostCheck, "ContactNotificationMethodData"},
        {Queue::HostCheck, "RestartData"},
        {Queue::HostCheck, "SystemCommandData"},
        {Queue::HostCheck, "CommentData"},
        {Queue::HostCheck, "ExternalCommandData"},
        {Queue::HostCheck, "NotificationData"},
        {Queue::HostCheck, "ProgramStatusData"},
        {Queue::HostCheck, "ContactStatusData"},
        {Queue::HostCheck, "ContactNotificationData"},
        {Queue::HostCheck, "EventHandlerData"},
        {Queue::HostCheck, "ProcessData"},
        {Queue::HostCheck, "BulkOCSP"},
        {Queue::HostCheck, "OCSP"},
        {Queue::HostCheck, "BulkOCHP"},
        {Queue::HostCheck, "OCHP"}};
} // namespace statusengine
