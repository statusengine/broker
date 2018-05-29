#include "RabbitmqClient.h"

#include "LogStream.h"
#include "RabbitmqConfiguration.h"
#include "Statusengine.h"

namespace statusengine {

    RabbitmqClient::RabbitmqClient(Statusengine *se, RabbitmqConfiguration *cfg) : MessageHandler(se), cfg(cfg) {
        conn = amqp_new_connection();
    }

    RabbitmqClient::~RabbitmqClient() {}

    bool RabbitmqClient::Connect() {
        if (cfg->ssl) {
            socket = amqp_ssl_socket_new(conn);
            amqp_ssl_socket_set_verify(socket, cfg->ssl_verify);
            if (cfg->ssl_cacert != "") {
                if (!amqp_ssl_socket_set_cacert(socket, cfg->ssl_cacert.c_str())) {
                    se->Log() << "Could not set ssl ca for rabbitmq connection" << eoem;
                    return false;
                }
            }
            if (cfg->ssl_cert != "") {
                if (cfg->ssl_key == "") {
                    se->Log() << "Please specify an ssl key for rabbitmq connection" << eoem;
                    return false;
                }
                if (!amqp_ssl_socket_set_key(socket, cfg->ssl_cert.c_str(), cfg->ssl_key.c_str())) {
                    se->Log() << "Could not set ssl cert and key for rabbitmq connection" << eoem;
                    return false;
                }
            }
            else if (cfg->ssl_key != "") {
                se->Log() << "Please specify an ssl cert for rabbitmq connection" << eoem;
                return false;
            }
        }
        else {
            socket = amqp_tcp_socket_new(conn);
        }

        if (socket == nullptr) {
            se->Log() << "Could not create amqp (rabbitmq) socket" << eoem;
            return false;
        }

        auto socketStatus = amqp_socket_open_noblock(socket, cfg->hostname.c_str(), cfg->port, cfg->timeout);
        if (socketStatus != AMQP_STATUS_OK) {
            se->Log() << "Could not connect to rabbitmq: " << socketStatus << eoem;
            return false;
        }

        amqp_login(conn, cfg->vhost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, cfg->username.c_str(),
                   cfg->password.c_str());

        auto channelStatus = amqp_channel_open(conn, 1);
        if (channelStatus == nullptr) {
            se->Log() << "Could not open rabbitmq channel: " << channelStatus << eoem;
            return false;
        }

        auto exchangeStatus =
            amqp_exchange_declare(conn, 1, amqp_cstring_bytes(cfg->exchange.c_str()), amqp_cstring_bytes("direct"), 0,
                                  cfg->durable_exchange, 0, 0, amqp_empty_table);
        if (exchangeStatus == nullptr) {
            se->Log() << "Could not declare rabbitmq exchange: " << exchangeStatus << eoem;
        }

        std::vector<std::string> queues = {"statusngin_contactstatus",
                                           "statusngin_ochp",
                                           "statusngin_servicestatus",
                                           "statusngin_hoststatus",
                                           "statusngin_contactnotificationdata",
                                           "statusngin_contactnotificationmethod",
                                           "statusngin_servicechecks",
                                           "statusngin_service_perfdata",
                                           "statusngin_ocsp",
                                           "statusngin_core_restart",
                                           "statusngin_notifications",
                                           "statusngin_processdata",
                                           "statusngin_hostchecks",
                                           "statusngin_programmstatus",
                                           "statusngin_logentries"};

        for (auto it = queues.begin(); it != queues.end(); ++it) {
            auto queueString = amqp_cstring_bytes((*it).c_str());
            auto queueStatus = amqp_queue_declare(conn, 1, queueString, 0, cfg->durable_queues, 0, 0, amqp_empty_table);
            if (queueStatus == nullptr) {
                se->Log() << "Could not declare rabbitmq queue \"" << *it << "\": " << queueStatus << eoem;
                return false;
            }
            auto bindStatus = amqp_queue_bind(conn, 1, queueString, amqp_cstring_bytes(cfg->exchange.c_str()),
                                              queueString, amqp_empty_table);
            if (bindStatus == nullptr) {
                se->Log() << "Could not bind rabbitmq queue \"" << *it << "\": " << bindStatus << eoem;
                return false;
            }
        }

        se->Log() << "Rabbitmq initialized" << eom;

        return true;
    }

    void RabbitmqClient::SendMessage(const std::string &queue, const std::string &message) const {
        amqp_bytes_t message_bytes;
        message_bytes.len = message.length();
        char *bytes = nullptr;
        bytes = strdup(message.c_str());
        message_bytes.bytes = reinterpret_cast<void *>(bytes);
        if (amqp_basic_publish(conn, 1, amqp_cstring_bytes(cfg->exchange.c_str()), amqp_cstring_bytes(queue.c_str()), 0,
                               0, NULL, message_bytes) != AMQP_STATUS_OK) {
            se->Log() << "Could not send message to rabbitmq" << eom;
        }
    }
} // namespace statusengine
