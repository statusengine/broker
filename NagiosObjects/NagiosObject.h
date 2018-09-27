#pragma once

#include <string>

#include <json.h>

namespace statusengine {
    class NagiosObject {
      public:
        NagiosObject();
        ~NagiosObject();
        std::string ToString();

        template <typename T> void SetData(std::string name, T value) {
            SetData<T>(name.c_str(), value);
        }
        template <typename T> void SetData(const char *name, T value) {
            SetJSONData(data, name, value);
        }

        json_object *data;

      protected:
        std::string EncodeString(char *value);

        void SetJSONData(json_object *obj, const char *name, std::string value);
        void SetJSONData(json_object *obj, const char *name, const char *value);
        void SetJSONData(json_object *obj, const char *name, int value);
        void SetJSONData(json_object *obj, const char *name, long int value);
        void SetJSONData(json_object *obj, const char *name, double value);
        void SetJSONData(json_object *obj, const char *name, long unsigned int value);
        void SetJSONData(json_object *obj, const char *name, NagiosObject *other);
        void SetJSONData(json_object *obj, const char *name, json_object *other);
    };

} // namespace statusengine
