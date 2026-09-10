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
        inline static char *get_json_string(yyjson_val *obj) {
            auto jsonChars = yyjson_get_str(obj);
            if (jsonChars == nullptr) {
                return nullptr;
            }
            auto jsonCharsLen = yyjson_get_len(obj);
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
        inline static char *get_json_string_c(yyjson_val *obj) {
            auto jsonChars = yyjson_get_str(obj);
            if (jsonChars == nullptr) {
                return nullptr;
            }
            return strndup(jsonChars, yyjson_get_len(obj));
        }

        void ProcessMessage(WorkerQueue workerQueue, const std::string &message) override {
            yyjson_doc *doc = yyjson_read(message.c_str(), message.length(), 0);
            if (doc == nullptr) {
                se->Log() << "Received non-json string '" << message
                          << "'. Ignoring..." << LogLevel::Warning;
            }
            else {
                ProcessMessage(workerQueue, yyjson_doc_get_root(doc));
                yyjson_doc_free(doc);
            }
        }

        void ProcessMessage(WorkerQueue workerQueue, yyjson_val *obj) override {
            if (workerQueue == WorkerQueue::OCHP) {
                yyjson_val *messages;
                if((messages = yyjson_obj_get(obj, "messages")) != nullptr) {
                    if (!yyjson_is_arr(messages)) {
                        se->Log() << "OCHP::messages is not an array. Ignoring..." << LogLevel::Warning;
                    }
                    else {
                        long unsigned int arrLen = yyjson_arr_size(messages);
                        for (long unsigned int i = 0; i < arrLen; i++) {
                            yyjson_val *arrObj = yyjson_arr_get(messages, i);
                            ProcessMessage(WorkerQueue::OCHP, arrObj);
                        }
                    }
                }
                else {
                    yyjson_val *hostcheck;
                    if((hostcheck = yyjson_obj_get(obj, "hostcheck")) != nullptr) {
                        ParseCheckResult(hostcheck);
                    }
                    else {
                        se->Log() << "OCHP Object doesn't contain a hostcheck value. Ignoring..."
                                  << LogLevel::Warning;
                    }
                }
            }
            else if (workerQueue == WorkerQueue::OCSP) {
                yyjson_val *messages;
                if((messages = yyjson_obj_get(obj, "messages")) != nullptr) {
                    if (!yyjson_is_arr(messages)) {
                        se->Log() << "OCSP::messages is not an array. Ignoring..." << LogLevel::Warning;
                    }
                    else {
                        long unsigned int arrLen = yyjson_arr_size(messages);
                        for (long unsigned int i = 0; i < arrLen; i++) {
                            yyjson_val *arrObj = yyjson_arr_get(messages, i);
                            ProcessMessage(WorkerQueue::OCSP, arrObj);
                        }
                    }
                }
                else {
                    yyjson_val *servicecheck;
                    if((servicecheck = yyjson_obj_get(obj, "servicecheck")) != nullptr) {
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
                yyjson_val *data = nullptr;
                bool haveCommand = false, haveData = false, haveList = false;
                size_t objIdx, objMax;
                yyjson_val *jsonKeyVal, *jsonValue;
                yyjson_obj_foreach(obj, objIdx, objMax, jsonKeyVal, jsonValue) {
                    std::string jsonKey(yyjson_get_str(jsonKeyVal), yyjson_get_len(jsonKeyVal));

                    if (jsonKey.compare("Command") == 0) {
                        command = std::string(yyjson_get_str(jsonValue), yyjson_get_len(jsonValue));
                        haveCommand = true;
                    }
                    else if (jsonKey.compare("Data") == 0) {
                        data = jsonValue;
                        haveData = true;
                    }
                    else if (jsonKey.compare("messages") == 0) {
                        if (!yyjson_is_arr(jsonValue)) {
                            se->Log() << "messages doesn't contain an array. Ignoring..." << LogLevel::Warning;
                        }
                        else {
                            long unsigned int arrLen = yyjson_arr_size(jsonValue);
                            for (long unsigned int i = 0; i < arrLen; i++) {
                                yyjson_val *arrObj = yyjson_arr_get(jsonValue, i);
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

        void ParseCheckResult(yyjson_val *obj) {
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

            // Direct lookups rather than iterating every key and running it down a chain
            // of string comparisons: one lookup per field instead of comparisons
            // proportional to keys times fields.
            yyjson_val *value = nullptr;
            if ((value = yyjson_obj_get(obj, "host_name")) != nullptr) {
                cr.host_name = get_json_string_c(value);
            }
            if ((value = yyjson_obj_get(obj, "service_description")) != nullptr) {
                cr.service_description = get_json_string_c(value);
            }
            if ((value = yyjson_obj_get(obj, "output")) != nullptr) {
                output = get_json_string_c(value);
            }
            if ((value = yyjson_obj_get(obj, "long_output")) != nullptr) {
                longOutput = get_json_string_c(value);
            }
            if ((value = yyjson_obj_get(obj, "perf_data")) != nullptr) {
                perfData = get_json_string_c(value);
            }
            if ((value = yyjson_obj_get(obj, "check_type")) != nullptr) {
                cr.check_type = yyjson_get_sint(value);
            }
            if ((value = yyjson_obj_get(obj, "return_code")) != nullptr) {
                cr.return_code = yyjson_get_sint(value);
            }
            if ((value = yyjson_obj_get(obj, "start_time")) != nullptr) {
                cr.start_time.tv_sec = yyjson_get_sint(value);
            }
            if ((value = yyjson_obj_get(obj, "end_time")) != nullptr) {
                cr.finish_time.tv_sec = yyjson_get_sint(value);
            }
            if ((value = yyjson_obj_get(obj, "early_timeout")) != nullptr) {
                cr.early_timeout = yyjson_get_sint(value);
            }
            if ((value = yyjson_obj_get(obj, "latency")) != nullptr) {
                cr.latency = yyjson_get_real(value);
            }
            if ((value = yyjson_obj_get(obj, "exited_ok")) != nullptr) {
                cr.exited_ok = yyjson_get_sint(value);
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

        void ParseScheduleCheck(yyjson_val *obj) {
            const char *hostname = nullptr;
            const char *service_description = nullptr;
            time_t schedule_time = 0;
            auto _ = gsl::finally([&] {
                delete[] hostname;
                delete[] service_description;
            });
            size_t objIdx, objMax;
            yyjson_val *jsonKeyVal, *jsonValue;
            yyjson_obj_foreach(obj, objIdx, objMax, jsonKeyVal, jsonValue) {
                std::string jsonKey(yyjson_get_str(jsonKeyVal), yyjson_get_len(jsonKeyVal));
                if (jsonKey.compare("host_name") == 0) {
                    hostname = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("service_description") == 0) {
                    service_description = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("schedule_time") == 0) {
                    schedule_time = yyjson_get_sint(jsonValue);
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

        void ParseDeleteDowntime(yyjson_val *obj) {
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
            size_t objIdx, objMax;
            yyjson_val *jsonKeyVal, *jsonValue;
            yyjson_obj_foreach(obj, objIdx, objMax, jsonKeyVal, jsonValue) {
                std::string jsonKey(yyjson_get_str(jsonKeyVal), yyjson_get_len(jsonKeyVal));
                if (jsonKey.compare("host_name") == 0) {
                    hostname = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("service_description") == 0) {
                    service_description = get_json_string(jsonValue);
                }
                else if (jsonKey.compare("start_time") == 0) {
                    start_time = yyjson_get_sint(jsonValue);
                }
                else if (jsonKey.compare("end_time") == 0) {
                    end_time = yyjson_get_sint(jsonValue);
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

        inline static void ParseRaw(yyjson_val *obj) {
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
                    /* Serialise now and keep the text: a yyjson value belongs to
                     * its own document, so holding the object would mean copying
                     * the whole tree into a batch document instead. */
                    bulkMessages.push_back(obj.ToString());
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
                /* Fixed envelope around the already serialised messages. */
                std::string msg;
                size_t total = 32;
                for (auto &m : bulkMessages) {
                    total += m.size() + 1;
                }
                msg.reserve(total);
                msg += "{\"messages\":[";
                bool first = true;
                for (auto &m : bulkMessages) {
                    if (!first) {
                        msg += ',';
                    }
                    first = false;
                    msg += m;
                }
                msg += "],\"format\":\"none\"}";
                for (auto &handler : *handlers) {
                    handler->SendMessage(queue, msg);
                }

                const auto &QueueId = QueueNameHandler::Instance().QueueIds();
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
        std::vector<std::string> bulkMessages;

        unsigned long maxBulkSize;
        unsigned long *globalBulkCounter;
        bool bulk;
    };
} // namespace statusengine
