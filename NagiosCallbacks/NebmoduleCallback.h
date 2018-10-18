#pragma once

#include "Nebmodule.h"

namespace statusengine {
    class Statusengine;

    class NebmoduleCallback {
      public:
        explicit NebmoduleCallback(NEBCallbackType cbType, Statusengine *se);
        NebmoduleCallback(NebmoduleCallback &&other) noexcept;
        virtual ~NebmoduleCallback() = default;

        virtual NEBCallbackType GetCallbackType();

        virtual void Callback(int event_type, void *data) = 0;

      protected:
        Statusengine *se;
        NEBCallbackType cbType;
    };

} // namespace statusengine
