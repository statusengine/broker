#include "Statusengine.h"

#include <utility>

#include "vendor/toml.hpp"

#include "Configuration.h"
#include "EventCallback/BulkMessageCallback.h"
#include "LogStream.h"
#include "Nebmodule.h"

namespace statusengine {

    Statusengine::Statusengine(nebmodule *handle, std::string configurationPath)
        : nebhandle(handle), configurationPath(configurationPath), ls(nullptr) {
        ls = new LogStream(this);
        callbacks = new std::map<NEBCallbackType, std::vector<NebmoduleCallback *> *>();
        configuration = new Configuration(this);
        testCB = nullptr;
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

        messageHandlers = new MessageHandlerList(this, configuration);
        if (!messageHandlers->Connect()) {
            return 1;
        }

        if (configuration->GetQueueHostStatus()) {
            RegisterCallback(new HostStatusCallback(this));
        }

        if (configuration->GetQueueHostCheck() || configuration->GetQueueOCHP()) {
            RegisterCallback(
                new HostCheckCallback(this, configuration->GetQueueHostCheck(), configuration->GetQueueOCHP()));
        }

        if (configuration->GetQueueServiceStatus()) {
            RegisterCallback(new ServiceStatusCallback(this));
        }

        if (configuration->GetQueueServiceCheck() || configuration->GetQueueOCSP() ||
            configuration->GetQueueServicePerfData()) {
            RegisterCallback(new ServiceCheckCallback(this, configuration->GetQueueServiceCheck(),
                                                      configuration->GetQueueOCSP(),
                                                      configuration->GetQueueServicePerfData()));
        }

        if (configuration->GetQueueStateChange()) {
            RegisterCallback(new StateChangeCallback(this));
        }

        if (configuration->GetQueueLogData()) {
            RegisterCallback(new LogDataCallback(this));
        }

        if (configuration->GetQueueSystemCommandData()) {
            RegisterCallback(new SystemCommandDataCallback(this));
        }

        if (configuration->GetQueueCommentData()) {
            RegisterCallback(new CommentDataCallback(this));
        }

        if (configuration->GetQueueExternalCommandData()) {
            RegisterCallback(new ExternalCommandDataCallback(this));
        }

        if (configuration->GetQueueAcknowledgementData()) {
            RegisterCallback(new AcknowledgementDataCallback(this));
        }

        if (configuration->GetQueueFlappingData()) {
            RegisterCallback(new FlappingDataCallback(this));
        }

        if (configuration->GetQueueDowntimeData()) {
            RegisterCallback(new DowntimeDataCallback(this));
        }

        if (configuration->GetQueueNotificationData()) {
            RegisterCallback(new NotificationDataCallback(this));
        }

        if (configuration->GetQueueProgramStatusData()) {
            RegisterCallback(new ProgramStatusDataCallback(this));
        }

        if (configuration->GetQueueContactStatusData()) {
            RegisterCallback(new ContactStatusDataCallback(this));
        }

        if (configuration->GetQueueContactNotificationData()) {
            RegisterCallback(new ContactNotificationDataCallback(this));
        }

        if (configuration->GetQueueContactNotificationMethodData()) {
            RegisterCallback(new ContactNotificationMethodDataCallback(this));
        }

        if (configuration->GetQueueEventHandlerData()) {
            RegisterCallback(new EventHandlerDataCallback(this));
        }

        RegisterCallback(new ProcessDataCallback(this, configuration->GetQueueRestartData(),
                                                 configuration->GetQueueProcessData(),
                                                 configuration->GetStartupScheduleMax()));

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
        delete testCB;
        delete configuration;
        delete messageHandlers;

        Log() << "unloading finished" << LogLevel::Info;
        delete ls;
    }

    void Statusengine::InitEventCallbacks() {
        Log() << "Initialize event callbacks" << LogLevel::Info;
        testCB = new BulkMessageCallback(this);
        RegisterEventCallback(testCB);
    }

    LogStream &Statusengine::Log() {
        return *ls;
    }

    void Statusengine::SendMessage(const std::string queue, const std::string message) const {
        messageHandlers->SendMessage(queue, message);
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
} // namespace statusengine
