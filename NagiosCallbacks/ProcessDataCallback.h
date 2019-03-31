#pragma once

#include <ctime>
#include <memory>

#include "MessageHandler/IMessageQueueHandler.h"
#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ProcessDataCallback : public NebmoduleCallback {
      public:
        explicit ProcessDataCallback(IStatusengine *se, time_t startupSchedulerMax);
        ProcessDataCallback(ProcessDataCallback &&other) noexcept;

        void Callback(int event_type, void *vdata) override;

      private:
        bool restartData;
        bool processData;
        time_t startupSchedulerMax;

        std::shared_ptr<IMessageQueueHandler> restartHandler;
        std::shared_ptr<IMessageQueueHandler> processHandler;
    };
} // namespace statusengine
