#ifndef NAGIOS_OBJECT_H
#define NAGIOS_OBJECT_H

#include "vendor/json.hpp"
#include <string>

#include "nebmodule.h"

using json = nlohmann::json;

namespace statusengine {
    class NagiosObject {
      public:
        NagiosObject(const json &j);

        json &GetData();

      protected:
        NagiosObject();

        template <typename T>
        void SetData(std::string name, T value, json *other = nullptr) {
            if (other == nullptr) {
                other = &Data;
            }
            (*other)[name] = value;
        }

        template <typename T>
        void SetData(std::string name, T *value, json *other = nullptr) {
            if (other == nullptr) {
                other = &Data;
            }
            if (value == nullptr) {
                (*other)[name] = nullptr;
            }
            else {
                if (std::is_same<T, char *>::value) {
                    // fancy strings instead of c bullshit
                    (*other)[name] =
                        std::string(reinterpret_cast<char *>(value));
                }
                else {
                    (*other)[name] = value;
                }
            }
        }

        json Data;
    };

} // namespace statusengine

#endif // !NAGIOS_OBJECT_H
