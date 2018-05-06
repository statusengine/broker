#pragma once

#include <string>

#include <json.h>

#include "nebmodule.h"

namespace statusengine {
    class NagiosObject {
      public:
        std::string ToString();

      protected:
        NagiosObject();
        ~NagiosObject();

        std::string EncodeString(char *value);

        template <typename T>
        void SetData(std::string name, T value, json_object *obj = nullptr) {
            SetData<T>(name.c_str(), value, obj);
        }
        template <typename T>
        void SetData(const char *name, T value, json_object *obj = nullptr) {
            if (obj == nullptr) {
                obj = data;
            }
            SetJSONData(obj, name, value);
        }

        void SetJSONData(json_object *obj, const char *name, std::string value);
        void SetJSONData(json_object *obj, const char *name, const char *value);
        void SetJSONData(json_object *obj, const char *name, int value);
        void SetJSONData(json_object *obj, const char *name, long int value);
        void SetJSONData(json_object *obj, const char *name, double value);
        void SetJSONData(json_object *obj, const char *name,
                         long unsigned int value);
        void SetJSONData(json_object *obj, const char *name,
                         NagiosObject *other);
        void SetJSONData(json_object *obj, const char *name,
                         json_object *other);

        json_object *data;
    };

} // namespace statusengine
