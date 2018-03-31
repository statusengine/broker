#include "GearmanClient.h"

#include "LogStream.h"


namespace statusengine {

	GearmanClient::GearmanClient(std::ostream *os) : ls(*os) {
		client = gearman_client_create(nullptr);
		gearman_return_t ret = gearman_client_add_server(client, "localhost", 0);
		if (gearman_failed(ret)) {
			ls << "Gearman Connect Failed" << eoem;
		}
		else {
			ls << "Gearman connect success" << eom;
		}
	}

	GearmanClient::~GearmanClient() {
		gearman_client_free(client);
	}

	void GearmanClient::SendMessage(const std::string queue, const std::string message) const {
		auto ret = gearman_client_do_background(client, queue.c_str(), nullptr, message.c_str(), message.length(), nullptr);
		if (ret != GEARMAN_SUCCESS) {
			ls << "Could not write message to gearman queue" << eoem;
		}
	}
}
