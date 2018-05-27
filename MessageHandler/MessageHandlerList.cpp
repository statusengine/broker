#include "MessageHandlerList.h"

#include "Configuration.h"
#include "RabbitmqConfiguration.h"

#ifdef WITH_GEARMAN
#include "GearmanClient.h"
#endif

#ifdef WITH_RABBITMQ
#include "RabbitmqClient.h"
#endif

namespace statusengine {
    MessageHandlerList::MessageHandlerList(Statusengine *se, Configuration *cfg) : se(se) {
#ifdef WITH_GEARMAN
        std::vector<std::string> gearmanUrls = cfg->GetGearmanList();
        for (auto it = gearmanUrls.begin(); it != gearmanUrls.end(); ++it) {
            handlers.push_back(std::make_shared<GearmanClient>(se, *it));
        }
#endif
#ifdef WITH_RABBITMQ
        std::vector<RabbitmqConfiguration *> rabbitmqConfig = cfg->GetRabbitmqConfiguration();
        for (auto it = rabbitmqConfig.begin(); it != rabbitmqConfig.end(); ++it) {
            handlers.push_back(std::make_shared<RabbitmqClient>(se, *it));
        }
#endif
    }
    MessageHandlerList::~MessageHandlerList() {
        handlers.clear();
    }

    bool MessageHandlerList::Connect() {
        for (auto it = handlers.begin(); it != handlers.end(); ++it) {
            if (!(*it)->Connect()) {
                return false;
            }
        }
        return true;
    }

    void MessageHandlerList::SendMessage(const std::string &queue, const std::string &message) {
        for (auto it = handlers.begin(); it != handlers.end(); ++it) {
            (*it)->SendMessage(queue, message);
        }
    }

} // namespace statusengine
