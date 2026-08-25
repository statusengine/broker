#pragma once

#include <memory>
#include <set>
#include <string>
#include <cstring>

#include "Queue.h"
#include "NagiosObject.h"
#include "Configuration.h"
#include "IStatusengine.h"
#include "IMessageHandler.h"
#include "gsl.h"


namespace statusengine {
    class MessageHandler : public IMessageHandler {
      public:

        explicit MessageHandler(IStatusengine *se) : se(se) {}

        /**
         * Copy a json string into a buffer owned by the C++ side. Release it with delete[].
         */
        inline static char *get_json_string(json_object *obj) {
            auto jsonChars = json_object_get_string(obj);
            if (jsonChars == nullptr) {
                return nullptr;
            }
            auto jsonCharsLen = json_object_get_string_len(obj);
            char *chars = new char[jsonCharsLen + 1];
            std::memcpy(chars, jsonChars, jsonCharsLen);
            chars[jsonCharsLen] = 0; // set last byte to zero
            return chars;
        }

        /**
         * Copy a json string into a buffer allocated by the malloc family. Everything that
         * is stored in a check_result has to be allocated this way: free_check_result()
         * releases those strings with free(), which must not be paired with new[].
         */
        inline static char *get_json_string_c(json_object *obj) {
            auto jsonChars = json_object_get_string(obj);
            if (jsonChars == nullptr) {
                return nullptr;
            }
            return strndup(jsonChars, json_object_get_string_len(obj));
        }

        void ProcessMessage(WorkerQueue workerQueue, const std::string &message) override {
            json_object *obj = json_tokener_parse(message.c_str());
            if (obj == nullptr) {
                se->Log() << "Received non-json string '" << message
                          << "'. Ignoring..." << LogLevel::Warning;
            }
            else {
                ProcessMessage(workerQueue, obj);
                json_object_put(obj);
            }
        }

        void ProcessMessage(WorkerQueue workerQueue, json_object *obj) override {
            if (workerQueue == WorkerQueue::OCHP) {
                json_object *messages;
                if(json_object_object_get_ex(obj, "messages", &messages)) {
                    if (!json_object_is_type(messages, json_type_array)) {
                        se->Log() << "OCHP::messages is not an array. Ignoring..." << LogLevel::Warning;
                    }
                    else {
                        long unsigned int arrLen = json_object_array_length(messages);
                        for (long unsigned int i = 0; i < arrLen; i++) {
                            json_object *arrObj = json_object_array_get_idx(messages, i);
                            ProcessMessage(WorkerQueue::OCHP, arrObj);
                        }
                    }
                }
                else {
                    json_object *hostcheck;
                    if(json_object_object_get_ex(obj, "hostcheck", &hostcheck)) {
                        ParseCheckResult(hostcheck);
                    }
                    else {
                        se->Log() << "OCHP Object doesn't contain a hostcheck value. Ignoring..."
                                  << LogLevel::Warning;
                    }
                }
            }
            else if (workerQueue == WorkerQueue::OCSP) {
                json_object *messages;
                if(json_object_object_get_ex(obj, "messages", &messages)) {
                    if (!json_object_is_type(messages, json_type_array)) {
                        se->Log() << "OCSP::messages is not an array. Ignoring..." << LogLevel::Warning;
                    }
                    else {
                        long unsigned int arrLen = json_object_array_length(messages);
                        for (long unsigned int i = 0; i < arrLen; i++) {
                            json_object *arrObj = json_object_array_get_idx(messages, i);
                            ProcessMessage(WorkerQueue::OCSP, arrObj);
                        }
                    }
                }
                else {
                    json_object *servicecheck;
                    if(json_object_object_get_ex(obj, "servicecheck", &servicecheck)) {
                        ParseCheckResult(servicecheck);
                    }
                    else {
                        se->Log() << "OCSP Object doesn't contain a servicecheck value. Ignoring..."
                                  << LogLevel::Warning;
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
                    else if (jsonKey.compare("messages") == 0) {
                        if (!json_object_is_type(jsonValue, json_type_array)) {
                            se->Log() << "messages doesn't contain an array. Ignoring..." << LogLevel::Warning;
                        }
                        else {
                            long unsigned int arrLen = json_object_array_length(jsonValue);
                            for (long unsigned int i = 0; i < arrLen; i++) {
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
                        else if (command.compare("delete_downtime") == 0) {
                            ParseDeleteDowntime(data);
                        } 
                        else if (command.compare("raw") == 0) {
                            ParseRaw(data);
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
        }
      protected:
        IStatusengine *se;


        /**
         * Join output, long output and perf data the way naemon expects them in a single
         * plugin output string. The result is allocated with malloc, so that
         * free_check_result() can release it. Returns nullptr if there is nothing to join,
         * i.e. if at most one of the parts is present.
         */
        inline static char *BuildCheckOutput(const char *output, const char *longOutput, const char *perfData) {
            if (output == nullptr || (longOutput == nullptr && perfData == nullptr)) {
                return nullptr;
            }

            size_t strLen;
            if (longOutput == nullptr) {
                // output + pipe + perfData + newline + zero byte
                strLen = std::strlen(output) + std::strlen(perfData) + 3;
            }
            else if (perfData == nullptr) {
                // output + newline + longOutput + zero byte
                strLen = std::strlen(output) + std::strlen(longOutput) + 2;
            }
            else {
                // output + pipe + perfData + newline + longOutput + zero byte
                strLen = std::strlen(output) + std::strlen(perfData) + std::strlen(longOutput) + 3;
            }

            char *fullOutput = static_cast<char *>(malloc(strLen));
            if (fullOutput == nullptr) {
                return nullptr;
            }

            if (longOutput == nullptr) {
                std::snprintf(fullOutput, strLen, "%s|%s\n", output, perfData);
            }
            else if (perfData == nullptr) {
                std::snprintf(fullOutput, strLen, "%s\n%s", output, longOutput);
            }
            else {
                std::snprintf(fullOutput, strLen, "%s|%s\n%s", output, perfData, longOutput);
            }
            return fullOutput;
        }

        void ParseCheckResult(json_object *obj) {
            check_result cr;
            init_check_result(&cr);
            char *output = nullptr;
            char *longOutput = nullptr;
            char *perfData = nullptr;
            // These three stay ours unless ownership is explicitly handed to cr.output below,
            // in which case the local pointer is cleared. Everything still held here at the
            // end of the function is ours to release; free_check_result() takes care of cr.
            auto freeParts = gsl::finally([&] {
                free(output);
                free(longOutput);
                free(perfData);
            });

            json_object_object_foreach(obj, cKey, jsonValue) {
                std::string jsonKey(cKey);
                if (jsonKey.compare("host_name") == 0) {
                    cr.host_name = get_json_string_c(jsonValue);
                }
                else if (jsonKey.compare("service_description") == 0) {
                    cr.service_description = get_json_string_c(jsonValue);
                }
                else if (jsonKey.compare("output") == 0) {
                    output = get_json_string_c(jsonValue);
                }
                else if (jsonKey.compare("long_output") == 0) {
                    longOutput = get_json_string_c(jsonValue);
                }
                else if (jsonKey.compare("perf_data") == 0) {
                    perfData = get_json_string_c(jsonValue);
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

            cr.output = BuildCheckOutput(output, longOutput, perfData);
            if (cr.output == nullptr) {
                // Only a single part was given, hand it over instead of copying it.
                if (output != nullptr) {
                    cr.output = output;
                    output = nullptr;
                }
                else if (longOutput != nullptr) {
                    cr.output = longOutput;
                    longOutput = nullptr;
                }
            }

            if (cr.host_name == nullptr) {
                se->Log() << "Received hostcheck without host_name" << LogLevel::Warning;
            }
            else if (cr.output == nullptr) {
                se->Log() << "Received hostcheck without output" << LogLevel::Warning;
            }
            else {
                if (cr.service_description == nullptr) {
                    cr.object_check_type = HOST_CHECK;
                } else {
                    cr.object_check_type = SERVICE_CHECK;
                }
                process_check_result(&cr);
            }

            // frees host_name, service_description and output
            free_check_result(&cr);
        }

        void ParseScheduleCheck(json_object *obj) {
            const char *hostname = nullptr;
            const char *service_description = nullptr;
            time_t schedule_time = 0;
            auto _ = gsl::finally([&] {
                delete[] hostname;
                delete[] service_description;
            });
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
                Nebmodule::Instance().ScheduleHostCheckFixed(temp_host, schedule_time);
            }
            else {
                service *temp_service = find_service(hostname, service_description);
                if (temp_service == nullptr) {
                    se->Log() << "Received schedule_check command for unknown service " << service_description
                              << LogLevel::Warning;
                    return;
                }
                Nebmodule::Instance().ScheduleServiceCheckFixed(temp_service, schedule_time);
            }
        }

        void ParseDeleteDowntime(json_object *obj) {
            const char *hostname = nullptr;
            const char *service_description = nullptr;
            time_t start_time = 0;
            time_t end_time = 0;
            const char *comment = nullptr;
            auto _ = gsl::finally([&] {
                delete[] hostname;
                delete[] service_description;
                delete[] comment;
            });
            json_object_object_foreach(obj, cKey, jsonValue) {
                std::string jsonKey(cKey);
                if (jsonKey.compare("host_name") == 0) {
                    hostname = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("service_description") == 0) {
                    service_description = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("start_time") == 0) {
                    start_time = json_object_get_int64(jsonValue);
                }
                else if (jsonKey.compare("end_time") == 0) {
                    end_time = json_object_get_int64(jsonValue);
                }
                else if (jsonKey.compare("comment") == 0) {
                    comment = get_json_string(jsonValue);
                }
            }

            if (hostname == nullptr) {
                se->Log() << "Received delete_downtime command without hostname " << LogLevel::Warning;
                return;
            }

            Nebmodule::Instance().DeleteDowntime(hostname, service_description, start_time, end_time, comment);
        }

        inline static void ParseRaw(json_object *obj) {
            auto cmd = get_json_string(obj);
            process_external_command1(cmd);
            delete[] cmd;
        }
    };

    class MessageQueueHandler : public IMessageQueueHandler {
    public:
        explicit MessageQueueHandler(IStatusengine &se, IMessageHandlerList &mhlist, unsigned long maxBulkSize,
                                     unsigned long *globalBulkCounter, Queue queue,
                                     std::shared_ptr<std::vector<std::shared_ptr<IMessageHandler>>> handlers,
                                     bool bulk)
                : se(se), mhlist(mhlist), queue(queue), handlers(std::move(handlers)),
                  maxBulkSize(maxBulkSize), globalBulkCounter(globalBulkCounter), bulk(bulk) {}

        /**
        * SendMessage
        * @param JsonObjectContainer contains json object that will be deleted after sending the message
        */
        void SendMessage(NagiosObject &obj) override {
            if (bulk) {
                if(!obj.isEmpty()){
                    bulkMessages.push_back(std::unique_ptr<NagiosObject>(new NagiosObject(&obj)));
                    if (++(*globalBulkCounter) >= maxBulkSize) {
                        mhlist.FlushBulkQueue();
                    }
                }
            }
            else {
                if(!obj.isEmpty()){
                    std::string msg = obj.ToString();
                    for (auto &handler : *handlers) {
                        handler->SendMessage(queue, msg);
                    }
                }
            }
        }

        void FlushBulkQueue() override {
            if (!bulkMessages.empty()) {
                NagiosObject msgObj;
                json_object *arr = json_object_new_array();

                for (auto &obj : bulkMessages) {
                    json_object_array_add(arr, obj->GetDataCopy());
                }

                msgObj.SetData("messages", arr);
                msgObj.SetData("format", "none");

                std::string msg = msgObj.ToString();
                for (auto &handler : *handlers) {
                    handler->SendMessage(queue, msg);
                }

                auto QueueId = QueueNameHandler::Instance().QueueIds();
                se.Log() << "Sent bulk message (" << bulkMessages.size() << ") for queue "
                         << QueueId.at(queue) << LogLevel::Info;

                bulkMessages.clear();
            }
        }

    private:
        IStatusengine &se;
        IMessageHandlerList &mhlist;

        Queue queue;
        std::shared_ptr<std::vector<std::shared_ptr<IMessageHandler>>> handlers;
        std::vector<std::unique_ptr<NagiosObject>> bulkMessages;

        unsigned long maxBulkSize;
        unsigned long *globalBulkCounter;
        bool bulk;
    };
} // namespace statusengine
