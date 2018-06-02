#include "NebmoduleCallback.h"

namespace statusengine {
    NebmoduleCallback::NebmoduleCallback(NEBCallbackType cbType, Statusengine *se, int priority)
        : cbType(cbType), priority(priority), se(se) {}

    NEBCallbackType NebmoduleCallback::GetCallbackType() {
        return cbType;
    }

    int NebmoduleCallback::GetPriority() {
        return priority;
    }

} // namespace statusengine
