#include "NagiosObject.h"

#include <set>
#include <unicode/unistr.h>

namespace statusengine {

    NagiosObject::NagiosObject() { Data = json(); }

    NagiosObject::NagiosObject(const json &j) { Data = json(j); }

    json &NagiosObject::GetData() { return Data; }

    std::string NagiosObject::EncodeString(char *value) {
        std::string result;
        icu::UnicodeString(value, "UTF-8").toUTF8String(result);
        return result;
    }

} // namespace statusengine
