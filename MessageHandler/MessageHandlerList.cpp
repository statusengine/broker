#include "MessageHandlerList.h"

#include "Configuration.h"

#ifdef WITH_GEARMAN
#include "GearmanClient.h"
#endif

namespace statusengine {
    MessageHandlerList::MessageHandlerList(Statusengine *se, Configuration *cfg) : se(se) {
#ifdef WITH_GEARMAN
        std::vector<std::string> gearmanUrls = cfg->GetGearmanList();
        for (auto it = gearmanUrls.begin(); it != gearmanUrls.end(); ++it) {
            handlers.push_back(std::make_shared<GearmanClient>(se, *it));
        }
#endif
    }
    MessageHandlerList::~MessageHandlerList() {
        handlers.clear();
    }

    void MessageHandlerList::SendMessage(const std::string &queue, const std::string &message) {
        for (auto it = handlers.begin(); it != handlers.end(); ++it) {
            (*it)->SendMessage(queue, message);
        }
    }

} // namespace statusengine
