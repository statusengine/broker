#pragma once

#include <ctime>
#include <memory>
#include <string>
#include <vector>
#include <set>
#include <utility>
#include <map>
#include <toml.hpp>

#include "Queue.h"
#include "IStatusengine.h"

namespace statusengine {

    /**
     * Reads an optional key from a table, falling back to opt. A value of the wrong type is
     * reported and also falls back, so a single bad key does not take the whole config down.
     */
    template <typename T>
    T GetTomlDefault(IStatusengine &se, const toml::value &tab, const char *ky, T opt) {
        try {
            return toml::find_or<T>(tab, ky, opt);
        }
        catch (const toml::type_error &tte) {
            se.Log() << "Invalid configuration: Invalid value for key " << ky << LogLevel::Error;
        }
        return opt;
    }

    class MessageHandlerConfiguration {
    public:

        bool InitLoad(const toml::value &tbl) {
            for (auto &tableEntry : tbl.as_table()) {
                auto QueueName = QueueNameHandler::Instance().QueueNames();
                auto qName = QueueName.find(tableEntry.first);
                if (qName != QueueName.end()) {
                    try {
                        (*queues)[qName->second] = toml::get<std::string>(tableEntry.second);
                    }
                    catch (const toml::type_error &tte) {
                        se.Log() << "Invalid configuration: Invalid value for key " << tableEntry.first << LogLevel::Error;
                        return false;
                    }
                }
                else {
                    auto WorkerQueueName = QueueNameHandler::Instance().WorkerQueueNames();
                    auto wqName = WorkerQueueName.find(tableEntry.first);
                    if (wqName != WorkerQueueName.end()) {
                        try {
                            (*workerQueues)[wqName->second] = toml::get<std::string>(tableEntry.second);
                        }
                        catch (const toml::type_error &tte) {
                            se.Log() << "Invalid configuration: Invalid value for key " << tableEntry.first
                                     << LogLevel::Error;
                            return false;
                        }
                    }
                }
            }

            return Load(tbl);
        }

        const std::shared_ptr<std::map<Queue, std::string>> GetQueueNames() const {
            return queues;
        }

        std::shared_ptr<std::set<Queue>> GetQueues() const {
            auto queueIds = std::make_shared<std::set<Queue>>();
            for (auto &queue : *queues) {
                queueIds->insert(queue.first);
            }
            return queueIds;
        }

        const std::shared_ptr<std::map<WorkerQueue, std::string>> GetWorkerQueueNames() const {
            return workerQueues;
        }

        std::shared_ptr<std::set<WorkerQueue>> GetWorkerQueues() const {
            auto queueIds = std::make_shared<std::set<WorkerQueue>>();
            for (auto &queue : *workerQueues) {
                queueIds->insert(queue.first);
            }
            return queueIds;
        }

        virtual bool Load(const toml::value &tbl) = 0;

        virtual ~MessageHandlerConfiguration() = default;

    protected:
        explicit MessageHandlerConfiguration(IStatusengine &se) : se(se) {
            queues = std::make_shared<std::map<Queue, std::string>>();
            workerQueues = std::make_shared<std::map<WorkerQueue, std::string>>();
        }


        IStatusengine &se;

        std::shared_ptr<std::map<Queue, std::string>> queues;
        std::shared_ptr<std::map<WorkerQueue, std::string>> workerQueues;

    };


    class RabbitmqConfiguration : public MessageHandlerConfiguration {
    public:
        explicit RabbitmqConfiguration(IStatusengine &se)
                : MessageHandlerConfiguration(se), Port(5673), Timeout(), Exchange("statusengine"), SSL(false), SSLVerify(true)
                   {}

        virtual ~RabbitmqConfiguration() = default;

        bool Load(const toml::value &tbl) override {
            Hostname = GetTomlDefault(se, tbl, "Hostname", std::string(""));

            if (Hostname.empty()) {
                se.Log() << "Please specify a hostname in rabbitmq configuration" << LogLevel::Error;
                return false;
            }

            Port = GetTomlDefault(se, tbl, "Port", 5672);

            Vhost = GetTomlDefault(se, tbl, "Vhost", std::string("/"));
            Username = GetTomlDefault(se, tbl, "Username", std::string("statusengine"));
            Password = GetTomlDefault(se, tbl, "Password", std::string(""));
            if (Password.empty()) {
                se.Log() << "Warning, no password specified" << LogLevel::Warning;
            }

            int tov = GetTomlDefault(se, tbl, "Timeout", 30);
            Timeout.tv_sec = tov;
            Timeout.tv_usec = 0;

            Exchange = GetTomlDefault(se, tbl, "Exchange", std::string("statusengine"));

            // Durable by default. A queue that is neither durable nor exclusive is
            // RabbitMQ's deprecated transient_nonexcl_queues feature: 3.13 warns once per
            // broker start, 4.x refuses the declare outright, and the broker then fails to
            // connect at all (see Connect()). Durable has worked since AMQP 0-9-1, so it is
            // the only value that works on every supported broker version.
            //
            // This does not put events on disk. Queue durability and message persistence
            // are separate: durable stores the queue *definition*, while messages are only
            // written durably when the publisher marks them persistent - and SendMessage
            // passes properties=nullptr, i.e. transient. So the queues still buffer in RAM
            // and a broker restart still empties them, which is the intended behaviour.
            //
            // The exchange follows the queues: a transient exchange loses its bindings on a
            // broker restart while the durable queues survive, and keeping the pair
            // consistent costs nothing, both being metadata only.
            DurableExchange = GetTomlDefault(se, tbl, "DurableExchange", true);
            DurableQueues = GetTomlDefault(se, tbl, "DurableQueues", true);

            SSL = GetTomlDefault(se, tbl, "SSL", false);

            SSLVerify = GetTomlDefault(se, tbl, "SSL_verify", true);
            SSLCacert = GetTomlDefault(se, tbl, "SSL_cacert", std::string(""));
            SSLCert = GetTomlDefault(se, tbl, "SSL_cert", std::string(""));
            SSLKey = GetTomlDefault(se, tbl, "SSL_key", std::string(""));

            return true;
        }

        std::string Hostname;
        int Port;
        timeval Timeout;
        std::string Vhost;
        std::string Username;
        std::string Password;
        std::string Exchange;
        bool DurableExchange;
        bool DurableQueues;
        bool SSL;
        bool SSLVerify;
        std::string SSLCacert;
        std::string SSLCert;
        std::string SSLKey;

    };

    class GearmanConfiguration : public MessageHandlerConfiguration {
    public:
        explicit GearmanConfiguration(IStatusengine &se) : MessageHandlerConfiguration(se) {}

        bool Load(const toml::value &tbl) override {
            try {
                URL = toml::find<std::string>(tbl, "URL");
            }
            catch (const std::exception &e) {
                se.Log() << "Invalid configuration: Invalid or missing value for key URL" << LogLevel::Error;
                return false;
            }

            return true;
        }

        std::string URL;
    };

    class Configuration {
      public:
        explicit Configuration(IStatusengine &se)
            : se(se), cfg(toml::table{}), bulkTable(toml::table{}), schedulerTable(toml::table{}),
              maxWorkerMessagesPerInterval(0), logLevel(LogLevel::Warning) {}
        ~Configuration() {
            rabbitmq.clear(); // shared_ptr
        }

        bool Load(const std::string &configurationPath) {
            try {
                cfg = toml::parse(configurationPath);
            }
            catch (const toml::syntax_error &ste) {
                se.Log() << "configuration syntax error: " << ste.what() << LogLevel::Error;
                return false;
            }
            catch (const std::exception &e) {
                se.Log() << "Could not read file: " << e.what() << LogLevel::Error;
                return false;
            }

            if (!ReadSection("Bulk", bulkTable) || !ReadSection("Scheduler", schedulerTable)) {
                return false;
            }

            toml::value logTable;
            if (!ReadSection("Log", logTable)) {
                return false;
            }
            auto logLevelStr = GetTomlDefault(se, logTable, "Level", std::string("Warning"));
            if (logLevelStr == "Info") {
                logLevel = LogLevel::Info;
            }
            else if (logLevelStr == "Warning") {
                logLevel = LogLevel::Warning;
            }
            else if (logLevelStr == "Error") {
                logLevel = LogLevel::Error;
            }
            else {
                se.Log() << "Invalid configuration: Unknown log level: " << logLevelStr << LogLevel::Error;
                return false;
            }

            if (bulkTable.contains("Queues")) {
                std::vector<std::string> bulkQueueList;
                try {
                    bulkQueueList = toml::find<std::vector<std::string>>(bulkTable, "Queues");
                }
                catch (const toml::type_error &tte) {
                    se.Log() << "Invalid configuration: Bulk::Queues isn't an array!" << LogLevel::Error;
                    return false;
                }
                const auto &QueueName = QueueNameHandler::Instance().QueueNames();
                for (auto &bulkQueueItem : bulkQueueList) {
                    auto queue = QueueName.find(bulkQueueItem);
                    if (queue == QueueName.end()) {
                        se.Log() << "Invalid configuration: Bulk::Queues contains an unknown queue identifier: "
                                 << bulkQueueItem << LogLevel::Error;
                        return false;
                    }
                    bulkQueues.insert(queue->second);
                }
            }

            if (!ReadHandlerConfigs<GearmanConfiguration>("Gearman", gearman) ||
                !ReadHandlerConfigs<RabbitmqConfiguration>("Rabbitmq", rabbitmq)) {
                return false;
            }

            toml::value workerTable;
            if (!ReadSection("Worker", workerTable)) {
                return false;
            }
            maxWorkerMessagesPerInterval = 1000000ul;
            if (workerTable.contains("MaxWorkerMessagesPerInterval")) {
                try {
                    maxWorkerMessagesPerInterval =
                        toml::find<unsigned long>(workerTable, "MaxWorkerMessagesPerInterval");
                }
                catch (const toml::type_error &tte) {
                    se.Log() << "Invalid configuration: Invalid value for key "
                             << "MaxWorkerMessagesPerInterval" << LogLevel::Error;
                    return false;
                }
            }

            se.Log() << "Finished loading config" << LogLevel::Info;
            se.Log() << "Gearman Clients: " << gearman.size() << LogLevel::Info;
            unsigned int counter = 0;
            for (auto &gearmanClient : gearman) {
                ++counter;
                auto queueNames = gearmanClient->GetQueueNames();
                for (auto &queueName : *queueNames) {
                    se.Log() << "Gearman " << counter << " queue name: " << queueName.second << LogLevel::Info;
                }
            }

            se.Log() << "Rabbitmq Clients: " << rabbitmq.size() << LogLevel::Info;
            counter = 0;
            for (auto &rabbitClient : rabbitmq) {
                ++counter;
                auto queueNames = rabbitClient->GetQueueNames();
                for (auto &queueName : *queueNames) {
                    se.Log() << "Rabbitmq " << counter << " queue name: " << queueName.second << LogLevel::Info;
                }
            }

            return true;
        }

        time_t GetBulkFlushInterval() const {
            return GetTomlDefault(se, bulkTable, "FlushInterval", static_cast<time_t>(10));
        }

        unsigned long GetBulkMaximum() const {
            return GetTomlDefault(se, bulkTable, "Maximum", 200ul);
        }

        bool IsBulkQueue(Queue queue) const {
            return bulkQueues.find(queue) != bulkQueues.end();
        }

        time_t GetStartupScheduleMax() const {
            return GetTomlDefault(se, schedulerTable, "StartupScheduleMax", 0);
        }

        std::vector<std::shared_ptr<GearmanConfiguration>> *GetGearmanConfiguration() {
            return &gearman;
        }

        std::vector<std::shared_ptr<RabbitmqConfiguration>> *GetRabbitmqConfiguration() {
            return &rabbitmq;
        }

        unsigned long GetMaxWorkerMessagesPerInterval() const {
            return maxWorkerMessagesPerInterval;
        }

        LogLevel GetLogLevel() const {
            return logLevel;
        }

      private:
        /// Copies an optional top level table into target. An absent section leaves target
        /// as an empty table, so the lookups on it still work.
        bool ReadSection(const char *name, toml::value &target) {
            target = toml::value(toml::table{});
            if (!cfg.contains(name)) {
                return true;
            }
            const auto &section = cfg.at(name);
            if (!section.is_table()) {
                se.Log() << "Invalid configuration: " << name << " isn't a table!" << LogLevel::Error;
                return false;
            }
            target = section;
            return true;
        }

        /// Loads an optional array of tables, one connection configuration per entry.
        template <typename T>
        bool ReadHandlerConfigs(const char *name, std::vector<std::shared_ptr<T>> &out) {
            if (!cfg.contains(name)) {
                return true;
            }
            const auto &section = cfg.at(name);
            if (!section.is_array()) {
                se.Log() << "Invalid configuration: " << name << " isn't an Array of Tables!" << LogLevel::Error;
                return false;
            }
            for (const auto &entry : section.as_array()) {
                if (!entry.is_table()) {
                    se.Log() << "Invalid configuration: " << name << " isn't an Array of Tables!" << LogLevel::Error;
                    return false;
                }
                auto handlerConfig = std::make_shared<T>(se);
                if (!handlerConfig->InitLoad(entry)) {
                    return false;
                }
                out.push_back(handlerConfig);
            }
            return true;
        }

        IStatusengine &se;
        toml::value cfg;
        // Kept as (possibly empty) tables so the GetTomlDefault lookups below always have
        // something well formed to search, even when the section is absent.
        toml::value bulkTable;
        toml::value schedulerTable;

        std::vector<std::shared_ptr<RabbitmqConfiguration>> rabbitmq;
        std::vector<std::shared_ptr<GearmanConfiguration>> gearman;
        std::set<Queue> bulkQueues;

        unsigned long maxWorkerMessagesPerInterval;

        LogLevel logLevel;

    };
} // namespace statusengine
