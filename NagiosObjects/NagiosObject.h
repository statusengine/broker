#pragma once

#include <string>

#include <json.h>

namespace statusengine {
    class NagiosObject {
      public:
        explicit NagiosObject();
        /**
         * This is like a copy constructor, it increments the counter for data
         * @param data
         */
        explicit NagiosObject(json_object *data);
        /**
         * This is like a copy constructor, it increments the counter for obj->data
         * @param data
         */
        explicit NagiosObject(NagiosObject *obj);
        ~NagiosObject();
        std::string ToString();

        /**
         * Counter will be incremented
         * @return json_object*
         */
        json_object *GetDataCopy();

        template <typename T> void SetData(std::string name, T value) {
            SetData<T>(name.c_str(), value);
        }
        template <typename T> void SetData(const char *name, T value) {
            SetJSONData(data, name, value);
        }

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

        json_object *data;
    };

} // namespace statusengine
