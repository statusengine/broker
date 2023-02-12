#pragma once

#include <cstring>
#include <memory>
#include <rapidjson/document.h>
#include <rapidjson/reader.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <set>
#include <string>

#include "../Configuration.h"
#include "../IStatusengine.h"
#include "../Queue.h"
#include "../Utility.h"
#include "IBulkMessageCounter.h"
#include "IMessageHandler.h"

namespace statusengine {
class MessageHandler : public IMessageHandler {
  public:
    explicit MessageHandler(IStatusengine &se) : se(se) {}

    /**
     * Naemon takes memory ownership of the json strings over and free()'s them
     * later. Therefore we have to copy them.
     */
    inline static char *copy_json_value(rapidjson::Value &value) {
        auto jsonChars = value.GetString();
        auto jsonCharsLen = value.GetStringLength();
        char *chars = static_cast<char *>(malloc(jsonCharsLen + 1));
        std::strncpy(chars, jsonChars, jsonCharsLen);
        chars[jsonCharsLen] = 0; // set last byte to zero
        return chars;
    }

    void ProcessMessage(WorkerQueue workerQueue,
                        const std::string &message) override {
        rapidjson::Document doc;
        doc.Parse(message);
        if (!doc.IsObject()) {
            se.Log() << "Received non-json string '" << message
                     << "'. Ignoring..." << LogLevel::Warning;
        } else {
            ProcessMessage(workerQueue, doc.GetObject());
        }
    }

    void ProcessMessage(WorkerQueue workerQueue,
                        rapidjson::Value::Object obj) override {
        auto findItr = obj.FindMember("messages");
        if (findItr != obj.MemberEnd()) {
            if (!findItr->value.IsArray()) {
                se.Log() << "messages is not an array. Ignoring..."
                         << LogLevel::Warning;
                return;
            }
            auto messages = findItr->value.GetArray();
            for (auto &message : messages) {
                if (!message.IsObject()) {
                    se.Log() << "messages contains non object "
                                "item in array. Ignoring..."
                             << LogLevel::Warning;
                } else {
                    ProcessMessage(workerQueue, message.GetObject());
                }
            }
            return;
        }

        const char *findData;
        switch (workerQueue) {
        case WorkerQueue::WorkerOCHP:
            findData = "hostcheck";
            break;
        case WorkerQueue::WorkerOCSP:
            findData = "servicecheck";
            break;
        case WorkerQueue::WorkerCommand:
            findData = "Data";
            break;
        }

        findItr = obj.FindMember(findData);
        if (findItr == obj.MemberEnd()) {
            se.Log() << "OCHP Object doesn't contain a " << findData
                     << " value. Ignoring..." << LogLevel::Warning;
        }
        switch (workerQueue) {
        case WorkerQueue::WorkerOCHP:
        case WorkerQueue::WorkerOCSP:
            if (!findItr->value.IsObject()) {
                se.Log() << findData << " is not an object value. Ignoring..."
                         << LogLevel::Warning;
                return;
            }
            ParseCheckResult(findItr->value.GetObject());
            return;
        default:
            break;
        }

        auto findCommandItr = obj.FindMember("Command");
        if (findCommandItr == obj.MemberEnd() ||
            !findCommandItr->value.IsString()) {
            se.Log() << "Command Object is missing Command. "
                        "Ignoring..."
                     << LogLevel::Warning;
            return;
        }

        std::string command = findCommandItr->value.GetString();
        if (command.compare("raw")) {
            if (!findItr->value.IsString()) {
                se.Log() << "Command raw is not a string. "
                            "Ignoring..."
                         << LogLevel::Warning;
                return;
            }
        } else {
            if (!findItr->value.IsObject()) {
                se.Log() << "Command is not an object. "
                            "Ignoring..."
                         << LogLevel::Warning;
                return;
            }
        }

        if (command.compare("check_result") == 0) {
            ParseCheckResult(findItr->value.GetObject());
        } else if (command.compare("schedule_check") == 0) {
            ParseScheduleCheck(findItr->value.GetObject());
        } else if (command.compare("delete_downtime") == 0) {
            ParseDeleteDowntime(findItr->value.GetObject());
        } else if (command.compare("raw") == 0) {
            ParseRaw(findItr->value.GetString());
        }
    }

  protected:
    IStatusengine &se;

    void ParseCheckResult(rapidjson::Value::Object obj) {
        check_result cr;
        init_check_result(&cr);
        char *output = nullptr;
        char *longOutput = nullptr;
        char *perfData = nullptr;

        for (auto &member : obj) {
            std::string jsonKey(member.name.GetString());
            if (jsonKey.compare("host_name") == 0) {
                cr.host_name = copy_json_value(member.value);
            } else if (jsonKey.compare("service_description") == 0) {
                cr.service_description = copy_json_value(member.value);
            } else if (jsonKey.compare("output") == 0) {
                output = copy_json_value(member.value);
            } else if (jsonKey.compare("long_output") == 0) {
                longOutput = copy_json_value(member.value);
            } else if (jsonKey.compare("perf_data") == 0) {
                perfData = copy_json_value(member.value);
            } else if (jsonKey.compare("check_type") == 0) {
                cr.check_type = member.value.GetInt();
            } else if (jsonKey.compare("return_code") == 0) {
                cr.return_code = member.value.GetInt();
            } else if (jsonKey.compare("start_time") == 0) {
                cr.start_time.tv_sec = member.value.GetInt();
            } else if (jsonKey.compare("end_time") == 0) {
                cr.finish_time.tv_sec = member.value.GetInt();
            } else if (jsonKey.compare("early_timeout") == 0) {
                cr.early_timeout = member.value.GetInt();
            } else if (jsonKey.compare("latency") == 0) {
                cr.latency = member.value.GetDouble();
            } else if (jsonKey.compare("exited_ok") == 0) {
                cr.exited_ok = member.value.GetInt();
            }
        }

        std::stringstream fullOutput;
        if (output != nullptr) {
            fullOutput << output;
        }
        if (longOutput != nullptr) {
            if (output != nullptr)
                fullOutput << "\n";
            fullOutput << longOutput;
        }
        if (perfData != nullptr) {
            fullOutput << "|" << perfData;
        }
        auto outputStr = fullOutput.str();
        cr.output = static_cast<char *>(malloc(outputStr.size() + 1));
        std::strncpy(cr.output, outputStr.c_str(), outputStr.size());
        cr.output[outputStr.size()] = 0;

        if (cr.host_name == nullptr) {
            se.Log() << "Received hostcheck without host_name"
                     << LogLevel::Warning;
        } else if (cr.output == nullptr) {
            se.Log() << "Received hostcheck without output"
                     << LogLevel::Warning;
        } else {
            if (cr.service_description == nullptr) {
                cr.object_check_type = HOST_CHECK;
            } else {
                cr.object_check_type = SERVICE_CHECK;
            }
            process_check_result(&cr);
        }

        // deletes hostname, service_description and output
        free_check_result(&cr);
        free(output);
        free(longOutput);
        free(perfData);
    }

    void ParseScheduleCheck(rapidjson::Value::Object obj) {
        const char *hostname = nullptr;
        const char *service_description = nullptr;
        time_t schedule_time = 0;

        for (auto &member : obj) {
            std::string jsonKey(member.name.GetString());
            if (jsonKey.compare("host_name") == 0) {
                hostname = member.value.GetString();
            } else if (jsonKey.compare("service_description") == 0) {
                service_description = member.value.GetString();
            } else if (jsonKey.compare("schedule_time") == 0) {
                schedule_time = member.value.GetInt64();
            }
        }

        if (hostname == nullptr || schedule_time == 0) {
            se.Log() << "Received schedule_check command without host_name and "
                        "schedule_time"
                     << LogLevel::Warning;
            return;
        }

        if (service_description == nullptr) {
            host *temp_host = find_host(hostname);
            if (temp_host == nullptr) {
                se.Log() << "Received schedule_check command for unknown host "
                         << *hostname << LogLevel::Warning;
                return;
            }
            se.GetNebmodule().ScheduleHostCheckFixed(temp_host, schedule_time);
        } else {
            service *temp_service = find_service(hostname, service_description);
            if (temp_service == nullptr) {
                se.Log()
                    << "Received schedule_check command for unknown service "
                    << *service_description << LogLevel::Warning;
                return;
            }
            se.GetNebmodule().ScheduleServiceCheckFixed(temp_service,
                                                        schedule_time);
        }
    }

    void ParseDeleteDowntime(rapidjson::Value::Object obj) {
        const char *hostname = nullptr;
        const char *service_description = nullptr;
        const char *comment = nullptr;
        time_t start_time = 0;
        time_t end_time = 0;

        for (auto &member : obj) {
            std::string jsonKey(member.name.GetString());
            if (jsonKey.compare("host_name") == 0) {
                hostname = member.value.GetString();
            } else if (jsonKey.compare("service_description") == 0) {
                service_description = member.value.GetString();
            } else if (jsonKey.compare("start_time") == 0) {
                start_time = member.value.GetInt64();
            } else if (jsonKey.compare("end_time") == 0) {
                end_time = member.value.GetInt64();
            } else if (jsonKey.compare("comment") == 0) {
                comment = member.value.GetString();
            }
        }

        if (hostname == nullptr) {
            se.Log() << "Received delete_downtime command without hostname "
                     << LogLevel::Warning;
            return;
        }

        se.GetNebmodule().DeleteDowntime(hostname, service_description,
                                         start_time, end_time, comment);
    }

    void ParseRaw(std::string str) {
        char *cmd = static_cast<char *>(malloc(str.size() + 1));
        std::strncpy(cmd, str.c_str(), str.size());
        cmd[str.size()] = 0;
        se.GetNebmodule().ProcessExternalCommand(cmd);
        free(cmd);
    }
};

class MessageQueueHandler : public IMessageQueueHandler {
  public:
    explicit MessageQueueHandler(
        IStatusengine &se, IBulkMessageCounter &bulkCounter, Queue queue,
        std::shared_ptr<std::vector<std::shared_ptr<IMessageHandler>>> handlers,
        bool bulk)
        : se(se), bulkCounter(bulkCounter), queue(queue),
          handlers(std::move(handlers)), bulk(bulk) {}

    void SendMessage(std::string s) override {
        if (bulk) {
            bulkMessages.push_back(s);
            bulkCounter.IncrementCounter();
        } else {
            for (auto &handler : *handlers) {
                handler->SendMessage(queue, s);
            }
        }
    }

    void FlushBulkQueue() override {
        if (!bulkMessages.empty()) {
            rapidjson::StringBuffer msgData;
            rapidjson::Writer<rapidjson::StringBuffer> msgObj(msgData);
            msgObj.StartObject();
            msgObj.Key("format");
            msgObj.String("none");
            msgObj.Key("messages");
            msgObj.StartArray();
            for (auto &obj : bulkMessages) {
                msgObj.RawValue(obj.data(), obj.size(),
                                rapidjson::Type::kObjectType);
            }
            msgObj.EndArray();
            msgObj.EndObject();

            for (auto &handler : *handlers) {
                handler->SendMessage(queue, msgData.GetString());
            }

            se.Log() << "Sent bulk message (" << bulkMessages.size()
                     << ") for queue " << wise_enum::to_string(queue)
                     << LogLevel::Info;

            // clearContainer<>(&bulkMessages);
            bulkMessages.clear();
        }
    }

  private:
    IStatusengine &se;
    IBulkMessageCounter &bulkCounter;

    Queue queue;
    std::shared_ptr<std::vector<std::shared_ptr<IMessageHandler>>> handlers;
    std::vector<std::string> bulkMessages;

    bool bulk;
};
} // namespace statusengine
