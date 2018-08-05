#pragma once

#include "vendor/toml.hpp"
#include <json.h>
#include <memory>
#include <set>
#include <string>

#include "Configuration/MessageHandlerConfiguration.h"
#include "Configuration/Queue.h"

namespace statusengine {
    class Statusengine;

    class MessageHandler {
      public:
        explicit MessageHandler(Statusengine *se);
        ~MessageHandler();

        void ProcessMessage(WorkerQueue workerQueue, const std::string &message);
        void ProcessMessage(WorkerQueue workerQueue, json_object *obj);

        virtual bool Connect() = 0;
        virtual bool Worker(unsigned long &counter) = 0;
        virtual void SendMessage(Queue queue, const std::string &message) = 0;

      protected:
        Statusengine *se;

        void ParseCheckResult(json_object *obj);
        void ParseScheduleCheck(json_object *obj);
        void ParseDowntime(json_object *obj);
        void ParseAcknowledge(json_object *obj);
        void ParseFlapDetection(json_object *obj);
        void ParseCustomNotification(json_object *obj);
        void ParseNotification(json_object *obj);
    };
} // namespace statusengine
