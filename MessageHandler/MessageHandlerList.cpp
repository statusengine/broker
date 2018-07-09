#include "MessageHandlerList.h"

#include "Configuration/Configuration.h"
#include "Configuration/GearmanConfiguration.h"
#include "Configuration/RabbitmqConfiguration.h"
#include "NagiosObjects/NagiosObject.h"

#ifdef WITH_GEARMAN
#include "GearmanClient.h"
#endif

#ifdef WITH_RABBITMQ
#include "RabbitmqClient.h"
#endif

namespace statusengine {
    MessageHandlerList::MessageHandlerList(Statusengine *se, Configuration *cfg)
        : se(se), maxBulkSize(0), globalBulkCounter(0) {
        maxBulkSize = cfg->GetBulkMaximum();
#ifdef WITH_GEARMAN
        auto gearmanConfig = cfg->GetGearmanConfiguration();
        for (auto it = gearmanConfig->begin(); it != gearmanConfig->end(); ++it) {
            handlers.push_back(std::make_shared<GearmanClient>(se, *it));
        }
#endif
#ifdef WITH_RABBITMQ
        auto rabbitmqConfig = cfg->GetRabbitmqConfiguration();
        for (auto it = rabbitmqConfig->begin(); it != rabbitmqConfig->end(); ++it) {
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

    void MessageHandlerList::SendBulkMessage(std::string queue, std::string message) {
        std::vector<std::string> *qmsg;
        try {
            qmsg = bulkMessages.at(queue);
        }
        catch (std::out_of_range &oor) {
            qmsg = new std::vector<std::string>();
            bulkMessages[queue] = qmsg;
        }
        qmsg->push_back(message);
        if (++globalBulkCounter >= maxBulkSize) {
            FlushBulkQueue();
        }
    } // namespace statusengine

    void MessageHandlerList::FlushBulkQueue() {
        for (auto &x : bulkMessages) {
            std::string queue = x.first;
            std::vector<std::string> *qmsg = x.second;
            if (qmsg->size() > 0) {
                NagiosObject obj;
                json_object *arr = json_object_new_array();
                for (auto &message : *qmsg) {
                    json_object_array_add(arr, json_object_new_string(message.c_str()));
                }
                obj.SetData<>("Messages", arr);
                obj.SetData<>("Compression", "plain");
                se->Log() << "Send bulk message (" << qmsg->size() << ") for queue " << queue << LogLevel::Info;
                SendMessage(queue, obj.ToString());
                qmsg->clear();
            }
        }
        globalBulkCounter = 0;
    }

} // namespace statusengine
