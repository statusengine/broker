#include "Statusengine.h"

#include <utility>

#include "Configuration.h"
#include "LogStream.h"
#include "MessageHandler/MessageHandlerList.h"
#include "NebmoduleCallback.h"
#include "NagiosObject.h"
#include "Nebmodule.h"
#include "Utility.h"

namespace statusengine {

    Statusengine::Statusengine(nebmodule *handle, std::string configurationPath)
        : nebhandle(handle), configurationPath(std::move(configurationPath)), messageHandler(nullptr), ls(),
          callbacks(), bulkCallback(nullptr), messageWorkerCallback(nullptr)  {
        configuration = new Configuration(*this);
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
        Log() << "License:\n"
                 "    statusengine - the missing event broker\n"
                 "    Copyright (C) 2019  The statusengine team\n"
                 "\n"
                 "    This program is free software; you can redistribute it and/or modify\n"
                 "    it under the terms of the GNU General Public License as published by\n"
                 "    the Free Software Foundation; either version 2 of the License, or\n"
                 "    (at your option) any later version.\n"
                 "\n"
                 "    This program is distributed in the hope that it will be useful,\n"
                 "    but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                 "    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                 "    GNU General Public License for more details.\n"
                 "\n"
                 "    You should have received a copy of the GNU General Public License along\n"
                 "    with this program; if not, write to the Free Software Foundation, Inc.,\n"
                 "    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n" << LogLevel::Info;

        if (!configuration->Load(configurationPath)) {
            return 1;
        }

        ls.SetLogLevel(configuration->GetLogLevel());

        messageHandler = new MessageHandlerList(*this, *configuration);
        if (!messageHandler->Connect()) {
            return 1;
        }

        RegisterCallback<StandardCallback<nebstruct_host_status_data, NagiosHostStatusData,
                NEBCALLBACK_HOST_STATUS_DATA, Queue::HostStatus>>({Queue::HostStatus});

        RegisterCallback<StandardCallback<nebstruct_service_status_data, NagiosServiceStatusData,
                NEBCALLBACK_SERVICE_STATUS_DATA, Queue::ServiceStatus>>({Queue::ServiceStatus});

        RegisterCallback<StandardCallback<nebstruct_log_data, NagiosLogData, NEBCALLBACK_LOG_DATA, Queue::LogData>>({Queue::LogData});

        RegisterCallback<StandardCallback<nebstruct_statechange_data, NagiosStateChangeData,
                NEBCALLBACK_STATE_CHANGE_DATA, Queue::StateChange>>({Queue::StateChange});

        RegisterCallback<StandardCallback<nebstruct_system_command_data, NagiosSystemCommandData,
                NEBCALLBACK_SYSTEM_COMMAND_DATA, Queue::SystemCommandData>>({Queue::SystemCommandData});

        RegisterCallback<StandardCallback<nebstruct_external_command_data, NagiosExternalCommandData,
                NEBCALLBACK_EXTERNAL_COMMAND_DATA, Queue::ExternalCommandData>>({Queue::ExternalCommandData});

        RegisterCallback<StandardCallback<nebstruct_comment_data, NagiosCommentData, NEBCALLBACK_COMMENT_DATA,
                Queue::CommentData>>({Queue::CommentData});

        RegisterCallback<StandardCallback<nebstruct_acknowledgement_data, NagiosAcknowledgementData,
                NEBCALLBACK_ACKNOWLEDGEMENT_DATA, Queue::AcknowledgementData>>({Queue::AcknowledgementData});

        RegisterCallback<StandardCallback<nebstruct_flapping_data, NagiosFlappingData,
                NEBCALLBACK_FLAPPING_DATA, Queue::FlappingData>>({Queue::FlappingData});

        RegisterCallback<StandardCallback<nebstruct_downtime_data, NagiosDowntimeData,
                NEBCALLBACK_DOWNTIME_DATA, Queue::DowntimeData>>({Queue::DowntimeData});

        RegisterCallback<StandardCallback<nebstruct_notification_data, NagiosNotificationData,
                NEBCALLBACK_NOTIFICATION_DATA, Queue::NotificationData>>({Queue::NotificationData});

        RegisterCallback<StandardCallback<nebstruct_program_status_data, NagiosProgramStatusData,
                NEBCALLBACK_PROGRAM_STATUS_DATA, Queue::ProgramStatusData>>({Queue::ProgramStatusData});

        RegisterCallback<StandardCallback<nebstruct_contact_status_data, NagiosContactStatusData,
                NEBCALLBACK_CONTACT_STATUS_DATA, Queue::ContactStatusData>>({Queue::ContactStatusData});

        RegisterCallback<StandardCallback<nebstruct_contact_notification_data, NagiosContactNotificationData,
                NEBCALLBACK_CONTACT_NOTIFICATION_DATA, Queue::ContactNotificationData>>({Queue::ContactNotificationData});

        RegisterCallback<StandardCallback<nebstruct_contact_notification_method_data, NagiosContactNotificationMethodData,
                NEBCALLBACK_CONTACT_NOTIFICATION_METHOD_DATA,
                Queue::ContactNotificationMethodData>>({Queue::ContactNotificationMethodData});

        RegisterCallback<StandardCallback<nebstruct_event_handler_data, NagiosEventHandlerData,
                NEBCALLBACK_EVENT_HANDLER_DATA, Queue::EventHandlerData>>({Queue::EventHandlerData});

        RegisterCallback<ServiceCheckCallback>({Queue::ServiceCheck, Queue::OCSP, Queue::ServicePerfData});

        RegisterCallback<HostCheckCallback>({Queue::HostCheck, Queue::OCHP});

        RegisterCallback<ProcessDataCallback>();

        messageHandler->InitComplete();

        return 0;
    }

    Statusengine::~Statusengine() {
        Log() << "unloading..." << LogLevel::Info;
        neb_deregister_module_callbacks(nebhandle);
        callbacks.clear();
        delete bulkCallback;
        delete messageWorkerCallback;
        delete configuration;
        delete messageHandler;

        Log() << "unloading finished" << LogLevel::Info;
    }

    void Statusengine::InitEventCallbacks() {
        Log() << "Initialize event callbacks" << LogLevel::Info;
        bulkCallback = new BulkMessageCallback(*this, configuration->GetBulkFlushInterval());
        messageWorkerCallback = new MessageWorkerCallback(*this, 1);
        RegisterEventCallback(bulkCallback);
        RegisterEventCallback(messageWorkerCallback);
    }

    LogStream &Statusengine::Log() {
        return ls;
    }

    void Statusengine::FlushBulkQueue() {
        messageHandler->FlushBulkQueue();
    }

    void Statusengine::SetModuleInfo(int modinfo, const std::string &text) {
        neb_set_module_info(nebhandle, modinfo, const_cast<char *>(text.c_str()));
    }

    void Statusengine::RegisterEventCallback(EventCallback *ecb) {
        Nebmodule::Instance().RegisterEventCallback(ecb);
    }

    int Statusengine::Callback(int event_type, void *data) {
        auto cbType = static_cast<NEBCallbackType>(event_type);
        auto cblist = callbacks.equal_range(cbType);

        for (auto cb = cblist.first; cb != cblist.second; ++cb) {
            cb->second->Callback(event_type, data);
        }

        return 0;
    }

    IMessageHandlerList *Statusengine::GetMessageHandler() const {
        return messageHandler;
    }
} // namespace statusengine
