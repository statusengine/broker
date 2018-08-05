#include "MessageHandler.h"

#include "Statusengine.h"

namespace statusengine {

    MessageHandler::MessageHandler(Statusengine *se) : se(se) {}
    MessageHandler::~MessageHandler(){};

    char *get_json_string(json_object *obj) {
        auto jsonChars = json_object_get_string(obj);
        auto jsonCharsLen = json_object_get_string_len(obj);
        char *chars = new char[jsonCharsLen + 1];
        strncpy(chars, jsonChars, jsonCharsLen);
        chars[jsonCharsLen] = 0; // set last byte to zero
        return chars;
    }

    void MessageHandler::ProcessMessage(WorkerQueue workerQueue, const std::string &message) {
        se->Log() << "Received Message: " << message << LogLevel::Info;

        json_object *obj = json_tokener_parse(message.c_str());

        if (workerQueue == WorkerQueue::OCHP) {
            json_object *hostcheck;
            json_object_object_get_ex(obj, "hostcheck", &hostcheck);
            if (hostcheck == nullptr) {
                se->Log() << "OCHP Object doesn't contain a hostcheck value. Ignoring..." << LogLevel::Warning;
            }
            else {
                ParseCheckResult(hostcheck);
            }
        }
        else if (workerQueue == WorkerQueue::OCSP) {
            json_object *servicecheck;
            json_object_object_get_ex(obj, "servicecheck", &servicecheck);
            if (servicecheck == nullptr) {
                se->Log() << "OCSP Object doesn't contain a servicecheck value. Ignoring..." << LogLevel::Warning;
            }
            else {
                ParseCheckResult(servicecheck);
            }
        }
        json_object_put(obj);
    }

    void MessageHandler::ParseCheckResult(json_object *obj) {
        check_result cr;
        init_check_result(&cr);
        char *hostName = nullptr;
        char *serviceDescription = nullptr;
        char *output = nullptr;
        char *longOutput = nullptr;
        char *fullOutput = nullptr;

        json_object_object_foreach(obj, cKey, jsonValue) {
            std::string jsonKey(cKey);
            if (jsonKey.compare("host_name") == 0) {
                hostName = get_json_string(jsonValue);
                cr.host_name = hostName;
            }
            else if (jsonKey.compare("service_description") == 0) {
                serviceDescription = get_json_string(jsonValue);
                cr.service_description = serviceDescription;
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

        if (cr.host_name != nullptr && cr.output != nullptr) {
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
} // namespace statusengine