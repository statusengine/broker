#include "NagiosObject.h"

#include <set>
#include <unicode/unistr.h>

namespace statusengine {

    NagiosObject::NagiosObject() { data = json_object_new_object(); }
    NagiosObject::~NagiosObject() {
        if (data != nullptr) {
            json_object_put(data);
        }
    }

    std::string NagiosObject::ToString() {
        return std::string(json_object_to_json_string(data));
    }

    std::string NagiosObject::EncodeString(char *value) {
        std::string result;
        icu::UnicodeString(value, "UTF-8").toUTF8String(result);
        return result;
    }

    void NagiosObject::SetJSONData(json_object *obj, const char *name,
                                   std::string value) {
        json_object_object_add(
            obj, name,
            json_object_new_string_len(value.c_str(), value.length()));
    }
    void NagiosObject::SetJSONData(json_object *obj, const char *name,
                                   const char *value) {
        if (value == nullptr) {
            json_object_object_add(obj, name, nullptr);
        }
        else {
            json_object_object_add(obj, name, json_object_new_string(value));
        }
    }
    void NagiosObject::SetJSONData(json_object *obj, const char *name,
                                   int value) {
        json_object_object_add(obj, name, json_object_new_int(value));
    }
    void NagiosObject::SetJSONData(json_object *obj, const char *name,
                                   long int value) {
        json_object_object_add(obj, name, json_object_new_int64(value));
    }
    void NagiosObject::SetJSONData(json_object *obj, const char *name,
                                   double value) {
        json_object_object_add(obj, name, json_object_new_double(value));
    }
    void NagiosObject::SetJSONData(json_object *obj, const char *name,
                                   long unsigned int value) {
        json_object_object_add(obj, name, json_object_new_int64(value));
    }
    void NagiosObject::SetJSONData(json_object *obj, const char *name,
                                   NagiosObject *other) {
        SetJSONData(obj, name, other->data);
        other->data = nullptr;
    }
    void NagiosObject::SetJSONData(json_object *obj, const char *name,
                                   json_object *other) {
        json_object_object_add(obj, name, other);
    }

} // namespace statusengine
