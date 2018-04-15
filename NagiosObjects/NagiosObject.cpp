#include "NagiosObject.h"

namespace statusengine {

    NagiosObject::NagiosObject() { Data = json(); }

    NagiosObject::NagiosObject(const json &j) { Data = json(j); }

    json &NagiosObject::GetData() { return Data; }
} // namespace statusengine
