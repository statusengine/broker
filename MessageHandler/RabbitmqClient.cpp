#include "RabbitmqClient.h"

#include "LogStream.h"
#include "RabbitmqConfiguration.h"
#include "Statusengine.h"

namespace statusengine {

    RabbitmqClient::RabbitmqClient(Statusengine *se, RabbitmqConfiguration *cfg)
        : MessageHandler(se), cfg(cfg), connected(false), conn(nullptr), socket(nullptr) {}

    RabbitmqClient::~RabbitmqClient() {
        CloseConnection(true);
    }

    bool RabbitmqClient::CloseConnection(bool quiet) {
        bool result = true;
        if (!CheckAMQPReply(amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS), "Closing amqp channel", quiet)) {
            result = false;
        }

        if (!CheckAMQPReply(amqp_connection_close(conn, AMQP_REPLY_SUCCESS), "Closing amqp connection", quiet)) {
            result = false;
        }
        if (amqp_destroy_connection(conn) < 0) {
            if (!quiet) {
                se->Log() << "Error ending amqp connection" << LogLevel::Error;
            }
            result = false;
        }
        return result;
    }

    bool RabbitmqClient::CheckAMQPReply(char const *context, bool quiet) {
        CheckAMQPReply(amqp_get_rpc_reply(conn), context, quiet);
    }

    bool RabbitmqClient::CheckAMQPReply(amqp_rpc_reply_t x, char const *context, bool quiet) {
        switch (x.reply_type) {
            case AMQP_RESPONSE_NORMAL:
                return true;

            case AMQP_RESPONSE_NONE:
                if (!quiet) {
                    se->Log() << context << ": missing RPC reply type!" << LogLevel::Error;
                }
                break;

            case AMQP_RESPONSE_LIBRARY_EXCEPTION:
                if (!quiet) {
                    se->Log() << context << ": " << amqp_error_string2(x.library_error) << LogLevel::Error;
                }
                break;

            case AMQP_RESPONSE_SERVER_EXCEPTION:
                switch (x.reply.id) {
                    case AMQP_CONNECTION_CLOSE_METHOD: {
                        amqp_connection_close_t *m = (amqp_connection_close_t *)x.reply.decoded;
                        if (!quiet) {
                            se->Log() << context << ": server connection error " << m->reply_code << ", message: "
                                      << std::string(reinterpret_cast<char *>(m->reply_text.bytes),
                                                     static_cast<int>(m->reply_text.len))
                                      << LogLevel::Error;
                        }
                    }
                    case AMQP_CHANNEL_CLOSE_METHOD: {
                        amqp_channel_close_t *m = (amqp_channel_close_t *)x.reply.decoded;
                        if (!quiet) {
                            se->Log() << context << ": server channel error " << m->reply_code << ", message: "
                                      << std::string(reinterpret_cast<char *>(m->reply_text.bytes),
                                                     static_cast<int>(m->reply_text.len))
                                      << LogLevel::Error;
                        }
                        break;
                    }
                    default:
                        if (!quiet) {
                            se->Log() << context << ": unknown server error, method id " << x.reply.id
                                      << LogLevel::Error;
                        }
                        break;
                }
                break;
        }

        return false;
    }

    bool RabbitmqClient::Connect() {
        Connect(false);
    }

    bool RabbitmqClient::Connect(bool quiet) {
        conn = amqp_new_connection();
        if (cfg->ssl) {
            socket = amqp_ssl_socket_new(conn);
            amqp_ssl_socket_set_verify(socket, cfg->ssl_verify);
            if (cfg->ssl_cacert != "") {
                if (!amqp_ssl_socket_set_cacert(socket, cfg->ssl_cacert.c_str())) {
                    if (!quiet) {
                        se->Log() << "Could not set ssl ca for rabbitmq connection" << LogLevel::Error;
                    }
                    return false;
                }
            }
            if (cfg->ssl_cert != "") {
                if (cfg->ssl_key == "") {
                    if (!quiet) {
                        se->Log() << "Please specify an ssl key for rabbitmq connection" << LogLevel::Error;
                    }
                    return false;
                }
                if (!amqp_ssl_socket_set_key(socket, cfg->ssl_cert.c_str(), cfg->ssl_key.c_str())) {
                    if (!quiet) {
                        se->Log() << "Could not set ssl cert and key for rabbitmq connection" << LogLevel::Error;
                    }
                    return false;
                }
            }
            else if (cfg->ssl_key != "") {
                if (!quiet) {
                    se->Log() << "Please specify an ssl cert for rabbitmq connection" << LogLevel::Error;
                }
                return false;
            }
        }
        else {
            socket = amqp_tcp_socket_new(conn);
        }

        if (socket == nullptr) {
            if (!quiet) {
                se->Log() << "Could not create amqp (rabbitmq) socket" << LogLevel::Error;
            }
            return false;
        }

        auto socketStatus = amqp_socket_open_noblock(socket, cfg->hostname.c_str(), cfg->port, cfg->timeout);
        if (socketStatus != AMQP_STATUS_OK) {
            if (!quiet) {
                se->Log() << "Could not connect to rabbitmq: " << socketStatus << LogLevel::Error;
            }
            return false;
        }

        if (!CheckAMQPReply(amqp_login(conn, cfg->vhost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                       cfg->username.c_str(), cfg->password.c_str()),
                            "amqp_login")) {
            return false;
        }

        amqp_channel_open(conn, 1);
        if (!CheckAMQPReply("Open amqp channel")) {
            return false;
        }

        amqp_exchange_declare(conn, 1, amqp_cstring_bytes(cfg->exchange.c_str()), amqp_cstring_bytes("direct"), 0,
                              cfg->durable_exchange, 0, 0, amqp_empty_table);
        if (!CheckAMQPReply("Declare amqp exchange")) {
            return false;
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
                                           "statusngin_logentries",
                                           "BulkOCSP",
                                           "BulkOCHP"};

        for (auto it = queues.begin(); it != queues.end(); ++it) {
            auto queueString = amqp_cstring_bytes((*it).c_str());
            amqp_queue_declare(conn, 1, queueString, 0, cfg->durable_queues, 0, 0, amqp_empty_table);
            if (!CheckAMQPReply(("Declare amqp queue " + *it).c_str())) {
                return false;
            }
            amqp_queue_bind(conn, 1, queueString, amqp_cstring_bytes(cfg->exchange.c_str()), queueString,
                            amqp_empty_table);
            if (!CheckAMQPReply(("Bind amqp queue " + *it).c_str())) {
                return false;
            }
        }

        connected = true;
        se->Log() << "Rabbitmq (re)connected" << LogLevel::Info;
        return true;
    }

    void RabbitmqClient::SendMessage(const std::string &queue, const std::string &message) {
        if (connected || Connect(true)) {
            amqp_bytes_t message_bytes;
            message_bytes.len = message.length();
            char *bytes = nullptr;
            bytes = strdup(message.c_str());
            message_bytes.bytes = reinterpret_cast<void *>(bytes);
            auto pubStatus = amqp_basic_publish(conn, 1, amqp_cstring_bytes(cfg->exchange.c_str()),
                                                amqp_cstring_bytes(queue.c_str()), 0, 0, NULL, message_bytes);
            if (pubStatus < 0) {
                connected = false;
                se->Log() << "Could not send message to rabbitmq: " << amqp_error_string2(pubStatus) << LogLevel::Error;
                CloseConnection(true);
            }
        }
    }
} // namespace statusengine
