#include "Statusengine.h"

#include <exception>
#include <fstream>
#include <memory>
#include <sstream>

#include "vendor/json.hpp"
#include "vendor/toml.hpp"

#include "Configuration.h"
#include "LogStream.h"

using json = nlohmann::json;

namespace statusengine {

    Statusengine::Statusengine(nebmodule *handle, std::string configurationPath)
        : nebhandle(handle), configurationPath(configurationPath) {}

    int Statusengine::Init() {
        SetModuleInfo(NEBMODULE_MODINFO_TITLE,
                      "Statusengine - the missing event broker");
        SetModuleInfo(NEBMODULE_MODINFO_AUTHOR, "Johannes Drummer");
        SetModuleInfo(NEBMODULE_MODINFO_TITLE,
                      "Copyright (c) 2018 - present Johannes Drummer");
        SetModuleInfo(NEBMODULE_MODINFO_VERSION, "4.0.0");
        SetModuleInfo(NEBMODULE_MODINFO_LICENSE, "GPL v2");
        SetModuleInfo(NEBMODULE_MODINFO_DESC,
                      "A powerful and flexible event broker");

        Log() << "the missing event broker" << eom;
        Log() << "This is the c++ version of statusengine event broker" << eom;

        try {
            const auto data = toml::parse(configurationPath);
            configuration = new Configuration(this, data);
        }
        catch (std::runtime_error &rte) {
            Log() << "Could not read file: " << rte.what() << eoem;
            return 1;
        }
        catch (toml::syntax_error &ste) {
            Log() << "configuration syntax error: " << ste.what() << eoem;
            return 1;
        }

        gearmanClients = configuration->GetGearmanClients(this);

        if (configuration->GetQueueHostStatus()) {
            cbHostStatus = new HostStatusCallback(this);
            RegisterCallback(cbHostStatus);
        }

        if (configuration->GetQueueHostCheck() ||
            configuration->GetQueueOCHP()) {
            cbHostCheck =
                new HostCheckCallback(this, configuration->GetQueueHostCheck(),
                                      configuration->GetQueueOCHP());
            RegisterCallback(cbHostCheck);
        }

        if (configuration->GetQueueServiceStatus()) {
            cbServiceStatus = new ServiceStatusCallback(this);
            RegisterCallback(cbServiceStatus);
        }

        if (configuration->GetQueueServiceCheck() ||
            configuration->GetQueueOCSP() ||
            configuration->GetQueueServicePerfData()) {
            cbServiceCheck = new ServiceCheckCallback(
                this, configuration->GetQueueServiceCheck(),
                configuration->GetQueueOCSP(),
                configuration->GetQueueServicePerfData());
            RegisterCallback(cbServiceCheck);
        }

        if (configuration->GetQueueStateChange()) {
            cbStateChange = new StateChangeCallback(this);
            RegisterCallback(cbStateChange);
        }

        if (configuration->GetQueueLogData()) {
            cbLogData = new LogDataCallback(this);
            RegisterCallback(cbLogData);
        }

        if (configuration->GetQueueSystemCommandData()) {
            cbSystemCommandData = new SystemCommandDataCallback(this);
            RegisterCallback(cbSystemCommandData);
        }

        if (configuration->GetQueueCommentData()) {
            cbCommentData = new CommentDataCallback(this);
            RegisterCallback(cbCommentData);
        }

        if (configuration->GetQueueExternalCommandData()) {
            cbExternalCommandData = new ExternalCommandDataCallback(this);
            RegisterCallback(cbExternalCommandData);
        }

        if (configuration->GetQueueAcknowledgementData()) {
            cbAcknowledgementData = new AcknowledgementDataCallback(this);
            RegisterCallback(cbAcknowledgementData);
        }

        if (configuration->GetQueueFlappingData()) {
            cbFlappingData = new FlappingDataCallback(this);
            RegisterCallback(cbFlappingData);
        }

        if (configuration->GetQueueDowntimeData()) {
            cbDowntimeData = new DowntimeDataCallback(this);
            RegisterCallback(cbDowntimeData);
        }

        if (configuration->GetQueueNotificationData()) {
            cbNotificationData = new NotificationDataCallback(this);
            RegisterCallback(cbNotificationData);
        }

        if (configuration->GetQueueProgramStatusData()) {
            cbProgramStatusData = new ProgramStatusDataCallback(this);
            RegisterCallback(cbProgramStatusData);
        }

        if (configuration->GetQueueContactStatusData()) {
            cbContactStatusData = new ContactStatusDataCallback(this);
            RegisterCallback(cbContactStatusData);
        }

        if (configuration->GetQueueContactNotificationData()) {
            cbContactNotificationData =
                new ContactNotificationDataCallback(this);
            RegisterCallback(cbContactNotificationData);
        }

        if (configuration->GetQueueContactNotificationMethodData()) {
            cbContactNotificationMethodData =
                new ContactNotificationMethodDataCallback(this);
            RegisterCallback(cbContactNotificationMethodData);
        }

        if (configuration->GetQueueEventHandlerData()) {
            cbEventHandlerData = new EventHandlerDataCallback(this);
            RegisterCallback(cbEventHandlerData);
        }

        if (configuration->GetQueueRestartData() ||
            configuration->GetQueueProcessData()) {
            cbProcessData = new ProcessDataCallback(
                this, configuration->GetQueueRestartData(),
                configuration->GetQueueProcessData());
            RegisterCallback(cbProcessData);
        }

        return 0;
    }

    Statusengine::~Statusengine() {
        Log() << "unloading..." << eom;
        neb_deregister_module_callbacks(nebhandle);

        delete cbHostStatus;
        delete cbHostCheck;
        delete cbServiceStatus;
        delete cbServiceCheck;
        delete cbStateChange;
        delete cbLogData;
        delete cbSystemCommandData;
        delete cbCommentData;
        delete cbExternalCommandData;
        delete cbAcknowledgementData;
        delete cbFlappingData;
        delete cbDowntimeData;
        delete cbNotificationData;
        delete cbProgramStatusData;
        delete cbContactStatusData;
        delete cbContactNotificationData;
        delete cbContactNotificationMethodData;
        delete cbEventHandlerData;
        delete cbProcessData;
        delete configuration;

        // Delete all gearman clients
        gearmanClients.clear();

        Log() << "unloading finished" << eom;
    }

    std::stringstream Statusengine::Log() { return std::stringstream(); }

    void Statusengine::SendMessage(const std::string queue,
                                   const std::string message) const {
        for (auto it = gearmanClients.begin(); it != gearmanClients.end();
             ++it) {
            (*it)->SendMessage(queue, message);
        }
    }

    void Statusengine::SetModuleInfo(int modinfo, std::string text) {
        neb_set_module_info(nebhandle, modinfo,
                            const_cast<char *>(text.c_str()));
    }

    void Statusengine::RegisterCallback(NEBCallbackType type,
                                        int callback(int, void *),
                                        int priority) {
        auto result =
            neb_register_callback(type, nebhandle, priority, callback);

        if (result != 0) {
            Log() << "Could not register callback: " << result << eoem;
        }
        else {
            Log() << "Register Callback successful" << eom;
        }
    }
} // namespace statusengine
