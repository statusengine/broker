#ifndef GEARMAN_CLIENT_H
#define GEARMAN_CLIENT_H

#include <sstream>

#include "libgearman-1.0/gearman.h"

namespace statusengine {
    class Statusengine;

    class GearmanClient {
      public:
        explicit GearmanClient(Statusengine *se, const std::string &url);
        ~GearmanClient();

        void SendMessage(const std::string &queue,
                         const std::string &message) const;

      private:
        Statusengine *se;
        gearman_client_st *client;
    };
} // namespace statusengine

#endif // !GEARMAN_CLIENT_H
