#ifndef GEARMAN_CLIENT
#define GEARMAN_CLIENT

#include "libgearman-1.0/gearman.h"

#include "Statusengine.h"

namespace statusengine {

	class GearmanClient {
	public:
		GearmanClient(std::ostream *os);
		~GearmanClient();

		void SendMessage(const std::string queue, const std::string message) const;
	private:
		std::ostream &ls;
		gearman_client_st *client;
	};
}

#endif // !GEARMAN_CLIENT
