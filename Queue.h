#pragma once

#include <map>


namespace statusengine {
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
        OCSP,
        OCHP
    };

    enum class WorkerQueue { OCSP, OCHP, Command };

    class QueueNameHandler {
    public:
        static QueueNameHandler& Instance() {
            static QueueNameHandler instance;
            return instance;
        }

        const std::map<std::string, Queue> &QueueNames() const {
            return queueNames;
        }

        const std::map<Queue, std::string> &QueueIds() const {
            return queueIds;
        }

        const std::map<std::string, WorkerQueue> &WorkerQueueNames() {
            return workerQueueNames;
        }

        const std::map<WorkerQueue, std::string> &WorkerQueueIds() {
            return workerQueueIds;
        }
    private:
        explicit QueueNameHandler() : queueNames(), queueIds() {
            queueNames = {
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
                {"OCSP", Queue::OCSP},
                {"OCHP", Queue::OCHP}};

            queueIds = {
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
                {Queue::OCSP, "OCSP"},
                {Queue::OCHP, "OCHP"}};

            workerQueueNames = {
                {"WorkerOCSP", WorkerQueue::OCSP},
                {"WorkerOCHP", WorkerQueue::OCHP},
                {"WorkerCommand", WorkerQueue::Command}};

            workerQueueIds = {
                {WorkerQueue::OCSP, "WorkerOCSP"},
                {WorkerQueue::OCHP, "WorkerOCHP"},
                {WorkerQueue::Command, "WorkerCommand"}};
        }

        std::map<std::string, Queue> queueNames;
        std::map<Queue, std::string> queueIds;
        std::map<std::string, WorkerQueue> workerQueueNames;
        std::map<WorkerQueue, std::string> workerQueueIds;
    };

} // namespace statusengine