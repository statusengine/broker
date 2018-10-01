#include "NebmoduleCallback.h"

namespace statusengine {
    NebmoduleCallback::NebmoduleCallback(NEBCallbackType cbType, Statusengine *se)
        : cbType(cbType), se(se) {}

    NEBCallbackType NebmoduleCallback::GetCallbackType() {
        return cbType;
    }
} // namespace statusengine
