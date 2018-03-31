#include "GearmanClient.h"

#include "LogStream.h"


namespace statusengine {

	GearmanClient::GearmanClient(std::ostream &os) : ls(os) {
		client = gearman_client_create(nullptr);
		gearman_return_t ret = gearman_client_add_server(client, "127.0.0.1", 4730);
		if (gearman_success(ret)) {
			ls << "Gearman connect success" << eom;
		}
		else {
			ls << "Gearman connect failed: " << gearman_client_error(client) << eoem;
		}
	}

	GearmanClient::~GearmanClient() {
		gearman_client_free(client);
	}

	void GearmanClient::SendMessage(const std::string queue, const std::string message) const {
		auto ret = gearman_client_do_background(client, queue.c_str(), nullptr, message.c_str(), message.length(), nullptr);
		if (!gearman_success(ret)) {
			ls << "Could not write message to gearman queue: " << gearman_client_error(client) << eoem;
		}
	}
}
