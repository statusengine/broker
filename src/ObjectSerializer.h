#pragma once

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <string_view>

#include "ISerializer.h"
#include "INebmodule.h"

namespace statusengine {

    typedef std::shared_ptr<rapidjson::StringBuffer> StringBufferPtr;

    template<class DataType>
    class ObjectSerializer : public ISerializer<DataType> {
    public:
        explicit ObjectSerializer(INebmodule &neb) : neb(neb), buffer(nullptr), writer(nullptr) {}

        std::string ToJson(DataType data) override {
            buffer = new rapidjson::StringBuffer();
            writer = new rapidjson::Writer<rapidjson::StringBuffer>(*buffer);
            writer->StartObject();
            Serializer(data);
            writer->EndObject();
            std::string result = buffer->GetString();
            delete writer;
            delete buffer;
            return result;
        }

    protected:
        inline void SetData(const char *name, const char *value) {
            writer->Key(name);
            if (value == nullptr) {
                writer->Null();
            } else {
                writer->String(value);
            }
        }

        inline void SetData(const char *name, const std::string_view value) {
            writer->Key(name);
            writer->String(value.data(), value.length(), false);
        }

        inline void SetData(const char *name, int64_t value) {
            writer->Key(name);
            writer->Int64(value);
        }

        inline void SetData(const char *name, int value) {
            writer->Key(name);
            writer->Int(value);
        }

        inline void SetData(const char *name, double value) {
            writer->Key(name);
            writer->Double(value);
        }

        inline void SetData(const char *name, uint64_t value) {
            writer->Key(name);
            writer->Uint64(value);
        }

        inline void StartChildObject(const char *name) {
            writer->Key(name);
            writer->StartObject();
        }

        inline void EndChildObject() {
            writer->EndObject();
        }

        inline void InsertJsonObject(const char *name, std::string_view value) {
            writer->Key(name);
            writer->RawValue(value.data(), value.size(), rapidjson::Type::kObjectType);
        }

        virtual void Serializer(DataType &data) = 0;

        INebmodule &neb;

    private:
        rapidjson::StringBuffer *buffer;
        rapidjson::Writer<rapidjson::StringBuffer> *writer;
    };

}
