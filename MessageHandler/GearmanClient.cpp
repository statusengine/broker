#include "GearmanClient.h"

#include "Configuration/GearmanConfiguration.h"
#include "LogStream.h"
#include "Statusengine.h"

#include <iostream>

namespace statusengine {

    GearmanClient::GearmanClient(Statusengine *se, std::shared_ptr<GearmanConfiguration> cfg)
        : MessageHandler(se), cfg(cfg) {
        client = gearman_client_create(nullptr);
        queueNames = cfg->GetQueueNames();
    }

    GearmanClient::~GearmanClient() {
        se->Log() << "Destroy gearman client" << LogLevel::Info;
        gearman_client_free(client);
    }

    bool GearmanClient::Connect() {
        gearman_return_t ret = gearman_client_add_servers(client, cfg->URL.c_str());
        if (gearman_success(ret)) {
            se->Log() << "Added gearman server connection" << LogLevel::Info;
            return true;
        }
        else {
            se->Log() << "Could not add gearman server: " << gearman_client_error(client) << LogLevel::Error;
            return false;
        }
    }

    void GearmanClient::SendMessage(Queue queue, const std::string &message) {
        auto queueName = queueNames->find(queue)->second;
        auto ret = gearman_client_do_background(client, queueName.c_str(), nullptr, message.c_str(), message.length(),
                                                nullptr);
        if (!gearman_success(ret)) {
            se->Log() << "Could not write message to gearman queue: " << gearman_client_error(client)
                      << LogLevel::Error;
        }
    }
} // namespace statusengine
