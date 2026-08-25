#pragma once

#include <map>
#include <string>

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

    /**
     * Maps between the queue enums and the identifiers used in statusengine.toml.
     *
     * The identifier of a queue is written down exactly once, in the tables below. Both
     * lookup directions are derived from them, so adding a queue means adding an enum value
     * and one table row.
     */
    class QueueNameHandler {
      public:
        static QueueNameHandler &Instance() {
            static QueueNameHandler instance;
            return instance;
        }

        const std::map<std::string, Queue> &QueueNames() const {
            return queueNames;
        }

        const std::map<Queue, std::string> &QueueIds() const {
            return queueIds;
        }

        const std::map<std::string, WorkerQueue> &WorkerQueueNames() const {
            return workerQueueNames;
        }

        const std::map<WorkerQueue, std::string> &WorkerQueueIds() const {
            return workerQueueIds;
        }

      private:
        template <typename T>
        struct Entry {
            T id;
            const char *name;
        };

        static const Entry<Queue> *QueueTable(size_t &count) {
            static const Entry<Queue> table[] = {
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
            count = sizeof(table) / sizeof(table[0]);
            return table;
        }

        static const Entry<WorkerQueue> *WorkerQueueTable(size_t &count) {
            static const Entry<WorkerQueue> table[] = {
                {WorkerQueue::OCSP, "WorkerOCSP"},
                {WorkerQueue::OCHP, "WorkerOCHP"},
                {WorkerQueue::Command, "WorkerCommand"}};
            count = sizeof(table) / sizeof(table[0]);
            return table;
        }

        template <typename T>
        static void Fill(const Entry<T> *table, size_t count, std::map<std::string, T> &byName,
                         std::map<T, std::string> &byId) {
            for (size_t i = 0; i < count; ++i) {
                byName[table[i].name] = table[i].id;
                byId[table[i].id] = table[i].name;
            }
        }

        QueueNameHandler() : queueNames(), queueIds(), workerQueueNames(), workerQueueIds() {
            // The table has to be fetched in its own statement: the order in which function
            // arguments are evaluated is unspecified, so passing QueueTable(count) and count
            // to the same call may read count before QueueTable() has set it.
            size_t queueCount = 0;
            const Entry<Queue> *queues = QueueTable(queueCount);
            Fill(queues, queueCount, queueNames, queueIds);

            size_t workerCount = 0;
            const Entry<WorkerQueue> *workers = WorkerQueueTable(workerCount);
            Fill(workers, workerCount, workerQueueNames, workerQueueIds);
        }

        std::map<std::string, Queue> queueNames;
        std::map<Queue, std::string> queueIds;
        std::map<std::string, WorkerQueue> workerQueueNames;
        std::map<WorkerQueue, std::string> workerQueueIds;
    };

} // namespace statusengine
