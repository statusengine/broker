#include "Statusengine.h"

#include <utility>

#include "vendor/toml.hpp"

#include "Configuration/Configuration.h"
#include "EventCallback/BulkMessageCallback.h"
#include "LogStream.h"
#include "MessageHandler/MessageHandlerList.h"
#include "NagiosCallbacks/HostCheckCallback.h"
#include "NagiosCallbacks/ProcessDataCallback.h"
#include "NagiosCallbacks/ServiceCheckCallback.h"
#include "NagiosCallbacks/StandardCallback.h"
#include "NagiosObjects/NagiosAcknowledgementData.h"
#include "NagiosObjects/NagiosCommentData.h"
#include "NagiosObjects/NagiosContactNotificationData.h"
#include "NagiosObjects/NagiosContactNotificationMethodData.h"
#include "NagiosObjects/NagiosContactStatusData.h"
#include "NagiosObjects/NagiosDowntimeData.h"
#include "NagiosObjects/NagiosEventHandlerData.h"
#include "NagiosObjects/NagiosExternalCommandData.h"
#include "NagiosObjects/NagiosFlappingData.h"
#include "NagiosObjects/NagiosHostStatusData.h"
#include "NagiosObjects/NagiosLogData.h"
#include "NagiosObjects/NagiosNotificationData.h"
#include "NagiosObjects/NagiosProgramStatusData.h"
#include "NagiosObjects/NagiosServiceStatusData.h"
#include "NagiosObjects/NagiosStateChangeData.h"
#include "NagiosObjects/NagiosSystemCommandData.h"
#include "Nebmodule.h"

namespace statusengine {

    Statusengine::Statusengine(nebmodule *handle, std::string configurationPath)
        : nebhandle(handle), configurationPath(configurationPath), ls(nullptr), bulkCallback(nullptr),
          messageWorkerCallback(nullptr) {
        ls = new LogStream(this);
        callbacks = new std::map<NEBCallbackType, std::vector<NebmoduleCallback *> *>();
        configuration = new Configuration(this);
    }

    int Statusengine::Init() {
        SetModuleInfo(NEBMODULE_MODINFO_TITLE, "Statusengine - the missing event broker");
        SetModuleInfo(NEBMODULE_MODINFO_AUTHOR, "Johannes Drummer");
        SetModuleInfo(NEBMODULE_MODINFO_TITLE, "Copyright (c) 2018 - present Johannes Drummer");
        SetModuleInfo(NEBMODULE_MODINFO_VERSION, "4.0.0");
        SetModuleInfo(NEBMODULE_MODINFO_LICENSE, "GPL v2");
        SetModuleInfo(NEBMODULE_MODINFO_DESC, "A powerful and flexible event broker");

        Log() << "the missing event broker" << LogLevel::Info;
        Log() << "This is the c++ version of statusengine event broker" << LogLevel::Info;

        if (!configuration->Load(configurationPath)) {
            return 1;
        }

        messageHandler = new MessageHandlerList(this, configuration);
        if (!messageHandler->Connect()) {
            return 1;
        }

        if (messageHandler->QueueExists(Queue::HostStatus)) {
            RegisterCallback(new StandardCallback<nebstruct_host_status_data, NagiosHostStatusData,
                                                  NEBCALLBACK_HOST_STATUS_DATA, Queue::HostStatus>(this));
        }

        if (messageHandler->QueueExists(Queue::ServiceStatus)) {
            RegisterCallback(new StandardCallback<nebstruct_service_status_data, NagiosServiceStatusData,
                                                  NEBCALLBACK_SERVICE_STATUS_DATA, Queue::ServiceStatus>(this));
        }

        if (messageHandler->QueueExists(Queue::LogData)) {
            RegisterCallback(
                new StandardCallback<nebstruct_log_data, NagiosLogData, NEBCALLBACK_LOG_DATA, Queue::LogData>(this));
        }

        if (messageHandler->QueueExists(Queue::StateChange)) {
            RegisterCallback(new StandardCallback<nebstruct_statechange_data, NagiosStateChangeData,
                                                  NEBCALLBACK_STATE_CHANGE_DATA, Queue::StateChange>(this));
        }

        if (messageHandler->QueueExists(Queue::SystemCommandData)) {
            RegisterCallback(new StandardCallback<nebstruct_system_command_data, NagiosSystemCommandData,
                                                  NEBCALLBACK_SYSTEM_COMMAND_DATA, Queue::SystemCommandData>(this));
        }

        if (messageHandler->QueueExists(Queue::ExternalCommandData)) {
            RegisterCallback(new StandardCallback<nebstruct_external_command_data, NagiosExternalCommandData,
                                                  NEBCALLBACK_EXTERNAL_COMMAND_DATA, Queue::ExternalCommandData>(this));
        }

        if (messageHandler->QueueExists(Queue::CommentData)) {
            RegisterCallback(new StandardCallback<nebstruct_comment_data, NagiosCommentData, NEBCALLBACK_COMMENT_DATA,
                                                  Queue::CommentData>(this));
        }

        if (messageHandler->QueueExists(Queue::AcknowledgementData)) {
            RegisterCallback(new StandardCallback<nebstruct_acknowledgement_data, NagiosAcknowledgementData,
                                                  NEBCALLBACK_ACKNOWLEDGEMENT_DATA, Queue::AcknowledgementData>(this));
        }

        if (messageHandler->QueueExists(Queue::FlappingData)) {
            RegisterCallback(new StandardCallback<nebstruct_flapping_data, NagiosFlappingData,
                                                  NEBCALLBACK_FLAPPING_DATA, Queue::FlappingData>(this));
        }

        if (messageHandler->QueueExists(Queue::DowntimeData)) {
            RegisterCallback(new StandardCallback<nebstruct_downtime_data, NagiosDowntimeData,
                                                  NEBCALLBACK_DOWNTIME_DATA, Queue::DowntimeData>(this));
        }

        if (messageHandler->QueueExists(Queue::NotificationData)) {
            RegisterCallback(new StandardCallback<nebstruct_notification_data, NagiosNotificationData,
                                                  NEBCALLBACK_NOTIFICATION_DATA, Queue::NotificationData>(this));
        }

        if (messageHandler->QueueExists(Queue::ProgramStatusData)) {
            RegisterCallback(new StandardCallback<nebstruct_program_status_data, NagiosProgramStatusData,
                                                  NEBCALLBACK_PROGRAM_STATUS_DATA, Queue::ProgramStatusData>(this));
        }

        if (messageHandler->QueueExists(Queue::ContactStatusData)) {
            RegisterCallback(new StandardCallback<nebstruct_contact_status_data, NagiosContactStatusData,
                                                  NEBCALLBACK_CONTACT_STATUS_DATA, Queue::ContactStatusData>(this));
        }

        if (messageHandler->QueueExists(Queue::ContactNotificationData)) {
            RegisterCallback(
                new StandardCallback<nebstruct_contact_notification_data, NagiosContactNotificationData,
                                     NEBCALLBACK_CONTACT_NOTIFICATION_DATA, Queue::ContactNotificationData>(this));
        }

        if (messageHandler->QueueExists(Queue::ContactNotificationMethodData)) {
            RegisterCallback(
                new StandardCallback<nebstruct_contact_notification_method_data, NagiosContactNotificationMethodData,
                                     NEBCALLBACK_CONTACT_NOTIFICATION_METHOD_DATA,
                                     Queue::ContactNotificationMethodData>(this));
        }

        if (messageHandler->QueueExists(Queue::EventHandlerData)) {
            RegisterCallback(new StandardCallback<nebstruct_event_handler_data, NagiosEventHandlerData,
                                                  NEBCALLBACK_EVENT_HANDLER_DATA, Queue::EventHandlerData>(this));
        }

        if (messageHandler->QueueExists(Queue::ServiceCheck) || messageHandler->QueueExists(Queue::OCSP) ||
            messageHandler->QueueExists(Queue::BulkOCSP) || messageHandler->QueueExists(Queue::ServicePerfData)) {
            RegisterCallback(new ServiceCheckCallback(this));
        }

        if (messageHandler->QueueExists(Queue::HostCheck) || messageHandler->QueueExists(Queue::OCHP) ||
            messageHandler->QueueExists(Queue::BulkOCHP)) {
            RegisterCallback(new HostCheckCallback(this));
        }

        if (messageHandler->QueueExists(Queue::RestartData) || messageHandler->QueueExists(Queue::ProcessData)) {
            RegisterCallback(new ProcessDataCallback(this, configuration->GetStartupScheduleMax()));
        }

        return 0;
    }

    Statusengine::~Statusengine() {
        Log() << "unloading..." << LogLevel::Info;
        neb_deregister_module_callbacks(nebhandle);

        for (auto const &x : *callbacks) {
            x.second->clear();
        }
        callbacks->clear();
        delete callbacks;
        delete bulkCallback;
        delete messageWorkerCallback;
        delete configuration;
        delete messageHandler;

        Log() << "unloading finished" << LogLevel::Info;
        delete ls;
    }

    void Statusengine::InitEventCallbacks() {
        Log() << "Initialize event callbacks" << LogLevel::Info;
        bulkCallback = new BulkMessageCallback(this, configuration->GetBulkFlushInterval());
        messageWorkerCallback = new MessageWorkerCallback(this, 1);
        RegisterEventCallback(bulkCallback);
        RegisterEventCallback(messageWorkerCallback);
    }

    LogStream &Statusengine::Log() {
        return *ls;
    }

    void Statusengine::FlushBulkQueue() {
        messageHandler->FlushBulkQueue();
    }

    void Statusengine::SetModuleInfo(int modinfo, std::string text) {
        neb_set_module_info(nebhandle, modinfo, const_cast<char *>(text.c_str()));
    }

    void Statusengine::RegisterCallback(NebmoduleCallback *cb) {
        std::vector<NebmoduleCallback *> *tpcb;
        try {
            tpcb = callbacks->at(cb->GetCallbackType());
        }
        catch (std::out_of_range &oor) {
            tpcb = new std::vector<NebmoduleCallback *>();
            callbacks->insert(std::make_pair(cb->GetCallbackType(), tpcb));
            Nebmodule::RegisterCallback(cb->GetCallbackType());
        }
        tpcb->push_back(cb);
    }

    void Statusengine::RegisterEventCallback(EventCallback *ecb) {
        Nebmodule::RegisterEventCallback(ecb);
    }

    int Statusengine::Callback(int event_type, void *data) {
        NEBCallbackType cbType = static_cast<NEBCallbackType>(event_type);
        try {
            std::vector<NebmoduleCallback *> *tpcb = callbacks->at(cbType);
            for (auto &x : *tpcb) {
                x->Callback(event_type, data);
            }
        }
        catch (std::out_of_range &oor) {
            Log() << "Could not find callback for " << event_type << LogLevel::Warning;
        }
        return 0;
    }

    MessageHandlerList *Statusengine::GetMessageHandler() const {
        return messageHandler;
    }
} // namespace statusengine
