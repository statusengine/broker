#pragma once

#include "vendor/toml.hpp"
#include <json.h>
#include <memory>
#include <set>
#include <string>

#include "Configuration.h"
#include "Queue.h"
#include "IStatusengine.h"


namespace statusengine {
    class MessageHandler {
      public:
        explicit MessageHandler(IStatusengine *se);

        void ProcessMessage(WorkerQueue workerQueue, const std::string &message);
        void ProcessMessage(WorkerQueue workerQueue, json_object *obj);

        virtual bool Connect() = 0;
        virtual bool Worker(unsigned long &counter) = 0;
        virtual void SendMessage(Queue queue, const std::string &message) = 0;

      protected:
        IStatusengine *se;

        void ParseCheckResult(json_object *obj);
        void ParseScheduleCheck(json_object *obj);
        void ParseRaw(json_object *obj);
    };
} // namespace statusengine
