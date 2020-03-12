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

    class MessageHandlerConfiguration {
    public:

        bool InitLoad(const toml::Table &tbl) {
            for (auto &tableEntry : tbl) {
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

        virtual bool Load(const toml::Table &tbl) = 0;

        virtual ~MessageHandlerConfiguration() = default;

    protected:
        explicit MessageHandlerConfiguration(IStatusengine &se) : se(se) {
            queues = std::make_shared<std::map<Queue, std::string>>();
            workerQueues = std::make_shared<std::map<WorkerQueue, std::string>>();
        }


        IStatusengine &se;

        std::shared_ptr<std::map<Queue, std::string>> queues;
        std::shared_ptr<std::map<WorkerQueue, std::string>> workerQueues;

        template <typename T> T GetTomlDefault(const toml::Table &tab, const char *ky, T &&opt) const {
            try {
                return toml::get_or(tab, ky, opt);
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Invalid value for key " << ky << LogLevel::Error;
            }
            return std::move(opt);
        }
    };


    class RabbitmqConfiguration : public MessageHandlerConfiguration {
    public:
        explicit RabbitmqConfiguration(IStatusengine &se)
                : MessageHandlerConfiguration(se), Port(5673), Timeout(), Exchange("statusengine"), SSL(false), SSLVerify(true)
                   {}

        virtual ~RabbitmqConfiguration() = default;

        bool Load(const toml::Table &tbl) override {
            Hostname = GetTomlDefault<>(tbl, "Hostname", std::string(""));

            if (Hostname.empty()) {
                se.Log() << "Please specify a hostname in rabbitmq configuration" << LogLevel::Error;
                return false;
            }

            Port = GetTomlDefault<>(tbl, "Port", 5672);

            Vhost = GetTomlDefault<>(tbl, "Vhost", std::string("/"));
            Username = GetTomlDefault<>(tbl, "Username", std::string("statusengine"));
            Password = GetTomlDefault<>(tbl, "Password", std::string(""));
            if (Password.empty()) {
                se.Log() << "Warning, no password specified" << LogLevel::Warning;
            }

            int tov = GetTomlDefault<>(tbl, "Timeout", 30);
            Timeout.tv_sec = tov;
            Timeout.tv_usec = 0;

            Exchange = GetTomlDefault<>(tbl, "Exchange", std::string("statusengine"));

            DurableExchange = GetTomlDefault<>(tbl, "DurableExchange", false);
            DurableQueues = GetTomlDefault<>(tbl, "DurableQueues", false);

            SSL = GetTomlDefault<>(tbl, "SSL", false);

            SSLVerify = GetTomlDefault<>(tbl, "SSL_verify", true);
            SSLCacert = GetTomlDefault<>(tbl, "SSL_cacert", std::string(""));
            SSLCert = GetTomlDefault<>(tbl, "SSL_cert", std::string(""));
            SSLKey = GetTomlDefault<>(tbl, "SSL_key", std::string(""));

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

        bool Load(const toml::Table &tbl) override {
            try {
                URL = toml::get<std::string>(tbl.at("URL"));
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Invalid value for key URL" << LogLevel::Error;
                return false;
            }

            return true;
        }

        std::string URL;
    };

    class Configuration {
      public:
        explicit Configuration(IStatusengine &se) : se(se), maxWorkerMessagesPerInterval(0), logLevel(LogLevel::Warning) {}
        ~Configuration() {
            rabbitmq.clear(); // shared_ptr
        }

        bool Load(const std::string &configurationPath) {
            try {
                cfg = toml::parse(configurationPath);
            }
            catch (std::runtime_error &rte) {
                se.Log() << "Could not read file: " << rte.what() << LogLevel::Error;
                return false;
            }
            catch (toml::syntax_error &ste) {
                se.Log() << "configuration syntax error: " << ste.what() << LogLevel::Error;
                return false;
            }

            try {
                bulkTable = cfg.at("Bulk").cast<toml::value_t::Table>();
            }
            catch (std::out_of_range &oor) {
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Bulk isn't a table!" << LogLevel::Error;
                return false;
            }

            try {
                auto logTable = cfg.at("Log").cast<toml::value_t::Table>();
                auto logLevelStr = toml::get_or<std::string>(logTable, "Level", "Warning");
                if (logLevelStr == "Info") {
                    logLevel = LogLevel::Info;
                }
                else if(logLevelStr == "Warning") {
                    logLevel = LogLevel::Warning;
                }
                else if(logLevelStr == "Error") {
                    logLevel = LogLevel::Error;
                }
                else {
                    se.Log() << "Invalid configuration: Unknown log level: " << logLevelStr << LogLevel::Error;
                    return false;
                }
            }
            catch (std::out_of_range &oor) {
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Log isn't a table!" << LogLevel::Error;
                return false;
            }

            try {
                auto QueueName = QueueNameHandler::Instance().QueueNames();
                std::vector<std::string> bulkQueueList = toml::get<std::vector<std::string>>(bulkTable.at("Queues"));
                for (auto &bulkQueueItem : bulkQueueList) {
                    try {
                        bulkQueues.insert(QueueName.at(bulkQueueItem));
                    }
                    catch (std::out_of_range &oor) {
                        se.Log() << "Invalid configuration: Bulk::Queues contains an unknown queue identifier: " << bulkQueueItem << LogLevel::Error;
                        return false;
                    }
                }
            }
            catch (std::out_of_range &oor) {
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Bulk::Queues isn't an array!" << LogLevel::Error;
                return false;
            }

            try {
                schedulerTable = cfg.at("Scheduler").cast<toml::value_t::Table>();
            }
            catch (std::out_of_range &oor) {
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Scheduler isn't a table!" << LogLevel::Error;
                return false;
            }

            try {
                std::vector<toml::Table> gearmans = toml::get<std::vector<toml::Table>>(cfg.at("Gearman"));
                for (auto &gearmanConfig : gearmans) {
                    auto gfg = std::make_shared<GearmanConfiguration>(se);
                    if (!gfg->InitLoad(gearmanConfig)) {
                        return false;
                    }
                    gearman.push_back(gfg);
                }
            }
            catch (const std::out_of_range &oor) {
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Gearman isn't an Array of Tables!" << LogLevel::Error;
                return false;
            }

            try {
                std::vector<toml::Table> rabbits = toml::get<std::vector<toml::Table>>(cfg.at("Rabbitmq"));
                for (auto &rabbitConfig : rabbits) {
                    auto rfg = std::make_shared<RabbitmqConfiguration>(se);
                    if (!rfg->InitLoad(rabbitConfig)) {
                        return false;
                    }
                    rabbitmq.push_back(rfg);
                }
            }
            catch (const std::out_of_range &oor) {
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Rabbitmq isn't an Array of Tables!" << LogLevel::Error;
                return false;
            }

            try {
                maxWorkerMessagesPerInterval = toml::get_or<unsigned long>(toml::get<toml::Table>(cfg.at("Worker")),
                                                                           "MaxWorkerMessagesPerInterval", 1000ul);
            }
            catch (const std::out_of_range &oor) {
                maxWorkerMessagesPerInterval = 1000ul;
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Invalid value for key "
                         << "MaxWorkerMessagesPerInterval" << LogLevel::Error;
                return false;
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
            return GetTomlDefault<>(bulkTable, "FlushInterval", static_cast<time_t>(10));
        }

        unsigned long GetBulkMaximum() const {
            return GetTomlDefault<>(bulkTable, "Maximum", 200ul);
        }

        bool IsBulkQueue(Queue queue) const {
            return bulkQueues.find(queue) != bulkQueues.end();
        }

        time_t GetStartupScheduleMax() const {
            return GetTomlDefault<>(schedulerTable, "StartupScheduleMax", 0);
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
        IStatusengine &se;
        toml::Table cfg;
        toml::Table bulkTable;
        toml::Table schedulerTable;

        std::vector<std::shared_ptr<RabbitmqConfiguration>> rabbitmq;
        std::vector<std::shared_ptr<GearmanConfiguration>> gearman;
        std::set<Queue> bulkQueues;

        unsigned long maxWorkerMessagesPerInterval;

        LogLevel logLevel;

        template <typename T> T GetTomlDefault(const toml::Table &tab, const char *ky, T &&opt) const {
            try {
                return toml::get_or(tab, ky, opt);
            }
            catch (const toml::type_error &tte) {
                se.Log() << "Invalid configuration: Invalid value for key " << ky << LogLevel::Error;
            }
            return opt;
        }
    };
} // namespace statusengine
