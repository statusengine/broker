#pragma once

#include <ctime>

#include "Nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ProcessDataCallback : public NebmoduleCallback {
      public:
        explicit ProcessDataCallback(Statusengine *se, bool restartData, bool processData, time_t startupSchedulerMax);

        virtual void Callback(int event_type, void *vdata);

      private:
        bool restartData;
        bool processData;
        time_t startupSchedulerMax;

        time_t RecalculateScheduleDelay(time_t check_interval);
    };
} // namespace statusengine
