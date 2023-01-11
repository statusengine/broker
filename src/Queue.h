#pragma once

#include <wise_enum.h>

namespace statusengine {

WISE_ENUM_CLASS((Queue, int), HostStatus, HostCheck, ServiceStatus,
                ServiceCheck, ServicePerfData, StateChange, LogData,
                AcknowledgementData, FlappingData, DowntimeData,
                ContactNotificationMethodData, RestartData, SystemCommandData,
                CommentData, ExternalCommandData, NotificationData,
                ProgramStatusData, ContactStatusData, ContactNotificationData,
                EventHandlerData, ProcessData, OCSP, OCHP)

WISE_ENUM_CLASS((WorkerQueue, int), WorkerOCSP, WorkerOCHP, WorkerCommand)
} // namespace statusengine
