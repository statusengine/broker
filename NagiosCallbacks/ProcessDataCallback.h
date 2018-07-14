#pragma once

#include <ctime>
#include <memory>

#include "MessageHandler/MessageQueueHandler.h"
#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ProcessDataCallback : public NebmoduleCallback {
      public:
        explicit ProcessDataCallback(Statusengine *se, time_t startupSchedulerMax);

        virtual void Callback(int event_type, void *vdata);

      private:
        bool restartData;
        bool processData;
        time_t startupSchedulerMax;

        time_t RecalculateScheduleDelay(time_t check_interval);

        std::shared_ptr<MessageQueueHandler> restartHandler;
        std::shared_ptr<MessageQueueHandler> processHandler;
    };
} // namespace statusengine
