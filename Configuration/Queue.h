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
        OCSP,
        OCHP
    };

    enum class WorkerQueue { OCSP, OCHP, Command };
} // namespace statusengine