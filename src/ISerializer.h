#pragma once

#include <string>

namespace statusengine {

    template<class DataType>
    class ISerializer {
    public:
        virtual ~ISerializer() = default;
        virtual std::string ToJson(DataType data) = 0;
    };

}
