#pragma once

#include <ctime>

#include "nebmodule.h"

#include "NebmoduleCallback.h"

namespace statusengine {
    class ProcessDataCallback : public NebmoduleCallback<nebstruct_process_data> {
      public:
        explicit ProcessDataCallback(Statusengine *se, bool restartData, bool processData, time_t startupSchedulerMax);

        virtual void Callback(int event_type, nebstruct_process_data *data);

      private:
        bool restartData;
        bool processData;
        time_t startupSchedulerMax;

        time_t RecalculateScheduleDelay(time_t check_interval);
    };
} // namespace statusengine
