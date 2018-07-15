#pragma once

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
        BulkOCSP,
        OCSP,
        BulkOCHP,
        OCHP
    };

    enum class WorkerQueue { OCSP, BulkOCSP, OCHP, BulkOCHP };
} // namespace statusengine