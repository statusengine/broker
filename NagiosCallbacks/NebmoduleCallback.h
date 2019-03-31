#pragma once

#include "Nebmodule.h"
#include "IStatusengine.h"

namespace statusengine {

    class NebmoduleCallback {
      public:
        explicit NebmoduleCallback(NEBCallbackType cbType, IStatusengine *se) : cbType(cbType), se(se) {}

        NebmoduleCallback(statusengine::NebmoduleCallback &&other) noexcept
                : cbType(other.cbType), se(other.se) {}

        virtual ~NebmoduleCallback() = default;

        virtual NEBCallbackType GetCallbackType() {
            return cbType;
        }

        virtual void Callback(int event_type, void *data) = 0;

      protected:
        IStatusengine *se;
        NEBCallbackType cbType;
    };

} // namespace statusengine
