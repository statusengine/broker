#include "MessageHandler.h"

#include "Statusengine.h"

namespace statusengine {

    MessageHandler::MessageHandler(Statusengine *se) : se(se) {}

    inline char *get_json_string(json_object *obj) {
        auto jsonChars = json_object_get_string(obj);
        auto jsonCharsLen = json_object_get_string_len(obj);
        char *chars = new char[jsonCharsLen + 1];
        strncpy(chars, jsonChars, jsonCharsLen);
        chars[jsonCharsLen] = 0; // set last byte to zero
        return chars;
    }

    void MessageHandler::ProcessMessage(WorkerQueue workerQueue, const std::string &message) {
        json_object *obj = json_tokener_parse(message.c_str());
        if (obj == nullptr) {
            se->Log() << "Received non-json string '''" << message
                      << "'''; \"'''\" is not part of the received message. Ignoring..." << LogLevel::Warning;
        }
        else {
            ProcessMessage(workerQueue, obj);
        }
    }

    void MessageHandler::ProcessMessage(WorkerQueue workerQueue, json_object *obj) {
        if (workerQueue == WorkerQueue::OCHP) {
            json_object *messages;
            json_object_object_get_ex(obj, "messages", &messages);
            if (messages == nullptr) {
                se->Log() << "OCHP Object doesn't contain a Messages array. Ignoring..." << LogLevel::Warning;
            }
            else {
                if (!json_object_is_type(messages, json_type_array)) {
                    se->Log() << "OCHP::Messages is not an array. Ignoring..." << LogLevel::Warning;
                }
                else {
                    auto arrLen = json_object_array_length(messages);
                    for (auto i = 0; i < arrLen; i++) {
                        json_object *arrObj = json_object_array_get_idx(messages, i);
                        json_object *hostcheck;
                        json_object_object_get_ex(obj, "hostcheck", &hostcheck);
                        if (hostcheck == nullptr) {
                            se->Log() << "OCHP Object doesn't contain a hostcheck value. Ignoring..."
                                      << LogLevel::Warning;
                        }
                        else {
                            ParseCheckResult(hostcheck);
                        }
                    }
                }
            }
        }
        else if (workerQueue == WorkerQueue::OCSP) {
            json_object *messages;
            json_object_object_get_ex(obj, "messages", &messages);
            if (messages == nullptr) {
                se->Log() << "OCSP Object doesn't contain a Messages array. Ignoring..." << LogLevel::Warning;
            }
            else {
                if (!json_object_is_type(messages, json_type_array)) {
                    se->Log() << "OCSP::Messages is not an array. Ignoring..." << LogLevel::Warning;
                }
                else {
                    auto arrLen = json_object_array_length(messages);
                    for (auto i = 0; i < arrLen; i++) {
                        json_object *arrObj = json_object_array_get_idx(messages, i);
                        ProcessMessage(WorkerQueue::OCSP, arrObj);
                        json_object *servicecheck;
                        json_object_object_get_ex(obj, "servicecheck", &servicecheck);
                        if (servicecheck == nullptr) {
                            se->Log() << "OCSP Object doesn't contain a servicecheck value. Ignoring..."
                                      << LogLevel::Warning;
                        }
                        else {
                            ParseCheckResult(servicecheck);
                        }
                    }
                }
            }
        }
        else if (workerQueue == WorkerQueue::Command) {
            std::string command;
            json_object *data = nullptr;
            bool haveCommand = false, haveData = false, haveList = false;
            json_object_object_foreach(obj, cKey, jsonValue) {
                std::string jsonKey(cKey);

                if (jsonKey.compare("Command") == 0) {
                    command = std::string(json_object_get_string(jsonValue), json_object_get_string_len(jsonValue));
                    haveCommand = true;
                }
                else if (jsonKey.compare("Data") == 0) {
                    data = jsonValue;
                    haveData = true;
                }
                else if (jsonKey.compare("CommandList") == 0) {
                    if (!json_object_is_type(jsonValue, json_type_array)) {
                        se->Log() << "CommandList doesn't contain an array. Ignoring..." << LogLevel::Warning;
                    }
                    else {
                        auto arrLen = json_object_array_length(jsonValue);
                        for (auto i = 0; i < arrLen; i++) {
                            json_object *arrObj = json_object_array_get_idx(jsonValue, i);
                            ProcessMessage(WorkerQueue::Command, arrObj);
                        }
                    }
                    haveList = true;
                }
            }
            if (!haveList) {
                if (haveData && haveCommand) {
                    if (command.compare("check_result") == 0) {
                        ParseCheckResult(data);
                    }
                    else if (command.compare("schedule_check") == 0) {
                        ParseScheduleCheck(data);
                    }
                    else if (command.compare("downtime") == 0) {
                        ParseDowntime(data);
                    }
                    else if (command.compare("acknowledge") == 0) {
                        ParseAcknowledge(data);
                    }
                    else if (command.compare("flap_detection") == 0) {
                        ParseFlapDetection(data);
                    }
                    else if (command.compare("custom_notification") == 0) {
                        ParseCustomNotification(data);
                    }
                    else if (command.compare("notification") == 0) {
                        ParseNotification(data);
                    }
                }
                else {
                    se->Log() << "Command Object is missing Command or Data. Ignoring..." << LogLevel::Warning;
                }
            }
        }
        else {
            se->Log() << "Received message for unknown worker queue" << LogLevel::Warning;
        }

        json_object_put(obj);
    }

    void MessageHandler::ParseCheckResult(json_object *obj) {
        check_result cr;
        init_check_result(&cr);
        char *output = nullptr;
        char *longOutput = nullptr;
        char *fullOutput = nullptr;

        json_object_object_foreach(obj, cKey, jsonValue) {
            std::string jsonKey(cKey);
            if (jsonKey.compare("host_name") == 0) {
                cr.host_name = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("service_description") == 0) {
                cr.service_description = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("output") == 0) {
                output = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("long_output") == 0) {
                longOutput = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("check_type") == 0) {
                cr.check_type = json_object_get_int64(jsonValue);
            }
            else if (jsonKey.compare("return_code") == 0) {
                cr.return_code = json_object_get_int64(jsonValue);
            }
            else if (jsonKey.compare("start_time") == 0) {
                cr.start_time.tv_sec = json_object_get_int64(jsonValue);
            }
            else if (jsonKey.compare("end_time") == 0) {
                cr.finish_time.tv_sec = json_object_get_int64(jsonValue);
            }
            else if (jsonKey.compare("early_timeout") == 0) {
                cr.early_timeout = json_object_get_int64(jsonValue);
            }
            else if (jsonKey.compare("latency") == 0) {
                cr.latency = json_object_get_double(jsonValue);
            }
            else if (jsonKey.compare("exited_ok") == 0) {
                cr.exited_ok = json_object_get_int64(jsonValue);
            }
        }

        if (output != nullptr && longOutput == nullptr) {
            cr.output = output;
        }
        else if (output != nullptr && longOutput != nullptr) {
            // we need a new string with size of strings + newline + zero byte
            auto strLen = strlen(output) + strlen(longOutput) + 2;
            fullOutput = new char[strLen];
            snprintf(fullOutput, strLen, "%s\n%s", output, longOutput);
            cr.output = fullOutput;
        }
        else if (longOutput != nullptr && output == nullptr) {
            cr.output = longOutput;
        }

        if (cr.host_name == nullptr) {
            se->Log() << "Received hostcheck without host_name" << LogLevel::Warning;
        }
        else if (cr.output == nullptr) {
            se->Log() << "Received hostcheck without output" << LogLevel::Warning;
        }
        else {
            process_check_result(&cr);
        }

        // deletes hostname, service_description and output
        free_check_result(&cr);
        if (output != nullptr && longOutput != nullptr) {
            // free_check_result only frees fulloutput in this case
            delete output;
            delete longOutput;
        }
    }

    void MessageHandler::ParseScheduleCheck(json_object *obj) {
        const char *hostname = nullptr;
        const char *service_description = nullptr;
        time_t schedule_time = 0;
        json_object_object_foreach(obj, cKey, jsonValue) {
            std::string jsonKey(cKey);
            if (jsonKey.compare("host_name") == 0) {
                hostname = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("service_description") == 0) {
                service_description = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("schedule_time") == 0) {
                schedule_time = json_object_get_int64(jsonValue);
            }
        }

        if (hostname == nullptr || schedule_time == 0) {
            se->Log() << "Received schedule_check command without host_name and schedule_time" << LogLevel::Warning;
            return;
        }

        if (service_description == nullptr) {
            host *temp_host = find_host(hostname);
            if (temp_host == nullptr) {
                se->Log() << "Received schedule_check command for unknown host " << hostname << LogLevel::Warning;
                return;
            }
            Nebmodule::ScheduleHostCheckFixed(temp_host, schedule_time);
        }
        else {
            service *temp_service = find_service(hostname, service_description);
            if (temp_service == nullptr) {
                se->Log() << "Received schedule_check command for unknown service " << service_description
                          << LogLevel::Warning;
                return;
            }
            Nebmodule::ScheduleServiceCheckFixed(temp_service, schedule_time);
        }
        delete hostname;
        delete service_description;
    }

    void MessageHandler::ParseDowntime(json_object *obj) {}

    void MessageHandler::ParseAcknowledge(json_object *obj) {
        const char *hostname = nullptr;
        const char *service_description = nullptr;
        const char *author = nullptr;
        const char *comment = nullptr;
        bool sticky = false;
        bool notify = false;
        bool persistent = false;
        json_object_object_foreach(obj, cKey, jsonValue) {
            std::string jsonKey(cKey);
            if (jsonKey.compare("host_name") == 0) {
                hostname = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("service_description") == 0) {
                service_description = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("author") == 0) {
                author = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("comment") == 0) {
                comment = get_json_string(jsonValue);
            }
            else if (jsonKey.compare("sticky") == 0) {
                sticky = json_object_get_boolean(jsonValue);
            }
            else if (jsonKey.compare("notify") == 0) {
                notify = json_object_get_boolean(jsonValue);
            }
            else if (jsonKey.compare("persistent") == 0) {
                persistent = json_object_get_boolean(jsonValue);
            }
        }

        if (hostname == nullptr) {
            se->Log() << "Received acknowledge command without host_name" << LogLevel::Warning;
            return;
        }

        if (author == nullptr) {
            author = strdup("unknown");
        }

        if (comment == nullptr) {
            comment = strdup("");
        }

        if (service_description == nullptr) {
            host *temp_host = find_host(hostname);
            if (temp_host == nullptr) {
                se->Log() << "Received acknowledge command for unknown host " << hostname << LogLevel::Warning;
                return;
            }
            if (!Nebmodule::AcknowledgeHost(temp_host, author, comment, sticky, notify, persistent)) {
                se->Log() << "Could not acknowledge host " << hostname << LogLevel::Info;
            }
        }
        else {
            service *temp_service = find_service(hostname, service_description);
            if (temp_service == nullptr) {
                se->Log() << "Received acknowledge command for unknown service " << hostname
                          << "::" << service_description << LogLevel::Warning;
                return;
            }
            if (!Nebmodule::AcknowledgeService(temp_service, author, comment, sticky, notify, persistent)) {
                se->Log() << "Could not acknowledge service " << hostname << "::" << service_description
                          << LogLevel::Info;
            }
        }

        delete hostname;
        delete service_description;
        delete author;
        delete comment;
    }

    void MessageHandler::ParseFlapDetection(json_object *obj) {}

    void MessageHandler::ParseCustomNotification(json_object *obj) {}

    void MessageHandler::ParseNotification(json_object *obj) {}

} // namespace statusengine