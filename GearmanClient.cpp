#include "GearmanClient.h"

#include "LogStream.h"

namespace statusengine {

    GearmanClient::GearmanClient(std::ostream &ls, const std::string url)
        : ls(ls) {
        client = gearman_client_create(nullptr);
        gearman_return_t ret = gearman_client_add_servers(client, url.c_str());
        if (gearman_success(ret)) {
            ls << "Added gearman server connection" << eom;
        }
        else {
            ls << "Could not add gearman server: "
               << gearman_client_error(client) << eoem;
        }
    }

    GearmanClient::~GearmanClient() {
        ls << "Destroy gearman client" << eom;
        gearman_client_free(client);
    }

    void GearmanClient::SendMessage(const std::string queue,
                                    const std::string message) const {
        auto ret = gearman_client_do_background(client, queue.c_str(), nullptr,
                                                message.c_str(),
                                                message.length(), nullptr);
        if (!gearman_success(ret)) {
            ls << "Could not write message to gearman queue: "
               << gearman_client_error(client) << eoem;
        }
    }
} // namespace statusengine
