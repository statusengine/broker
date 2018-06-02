#pragma once

#include "Nebmodule.h"

namespace statusengine {
    class Statusengine;

    class NebmoduleCallback {
      public:
        explicit NebmoduleCallback(NEBCallbackType cbType, Statusengine *se, int priority = 0);

        virtual NEBCallbackType GetCallbackType();

        virtual int GetPriority();

        virtual void Callback(int event_type, void *data) = 0;

      protected:
        Statusengine *se;
        int priority;
        NEBCallbackType cbType;
    };

} // namespace statusengine
