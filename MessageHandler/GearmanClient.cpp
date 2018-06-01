#include "GearmanClient.h"

#include "LogStream.h"
#include "Statusengine.h"

#include <iostream>

namespace statusengine {

    GearmanClient::GearmanClient(Statusengine *se, const std::string &url) : MessageHandler(se), url(url) {
        client = gearman_client_create(nullptr);
    }

    GearmanClient::~GearmanClient() {
        se->Log() << "Destroy gearman client" << LogLevel::Info;
        gearman_client_free(client);
    }

    bool GearmanClient::Connect() {
        gearman_return_t ret = gearman_client_add_servers(client, url.c_str());
        if (gearman_success(ret)) {
            se->Log() << "Added gearman server connection" << LogLevel::Info;
            return true;
        }
        else {
            se->Log() << "Could not add gearman server: " << gearman_client_error(client) << LogLevel::Error;
            return false;
        }
    }

    void GearmanClient::SendMessage(const std::string &queue, const std::string &message) const {
        auto ret =
            gearman_client_do_background(client, queue.c_str(), nullptr, message.c_str(), message.length(), nullptr);
        if (!gearman_success(ret)) {
            se->Log() << "Could not write message to gearman queue: " << gearman_client_error(client)
                      << LogLevel::Error;
        }
    }
} // namespace statusengine
