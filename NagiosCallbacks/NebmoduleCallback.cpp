#include "NebmoduleCallback.h"

namespace statusengine {
    NebmoduleCallback::NebmoduleCallback(NEBCallbackType cbType, Statusengine *se) : cbType(cbType), se(se) {}

    NebmoduleCallback::NebmoduleCallback(statusengine::NebmoduleCallback &&other) noexcept
        : cbType(other.cbType), se(other.se) {}

    NEBCallbackType NebmoduleCallback::GetCallbackType() {
        return cbType;
    }
} // namespace statusengine
