#include "RabbitmqClient.h"

#include "Configuration/RabbitmqConfiguration.h"
#include "LogStream.h"
#include "Statusengine.h"

namespace statusengine {

    RabbitmqClient::RabbitmqClient(Statusengine *se, std::shared_ptr<RabbitmqConfiguration> cfg)
        : MessageHandler(se), cfg(cfg), connected(false), conn(nullptr), socket(nullptr) {
        queueNames = cfg->GetQueueNames();
    }

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
        return CheckAMQPReply(amqp_get_rpc_reply(conn), context, quiet);
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
        return Connect(false);
    }

    bool RabbitmqClient::Connect(bool quiet) {
        conn = amqp_new_connection();
        if (cfg->SSL) {
            socket = amqp_ssl_socket_new(conn);
#ifndef WITH_RABBITMQ_CX080
            amqp_ssl_socket_set_verify_peer(socket, cfg->SSLVerify);
            amqp_ssl_socket_set_verify_hostname(socket, cfg->SSLVerify);
#else
            amqp_ssl_socket_set_verify(socket, cfg->SSLVerify);
#endif // WITH_RABBITMQ_CX080
            if (cfg->SSLCacert != "") {
                if (!amqp_ssl_socket_set_cacert(socket, cfg->SSLCacert.c_str())) {
                    if (!quiet) {
                        se->Log() << "Could not set ssl ca for rabbitmq connection" << LogLevel::Error;
                    }
                    return false;
                }
            }
            if (cfg->SSLCert != "") {
                if (cfg->SSLKey == "") {
                    if (!quiet) {
                        se->Log() << "Please specify an ssl key for rabbitmq connection" << LogLevel::Error;
                    }
                    return false;
                }
                if (!amqp_ssl_socket_set_key(socket, cfg->SSLCert.c_str(), cfg->SSLKey.c_str())) {
                    if (!quiet) {
                        se->Log() << "Could not set ssl cert and key for rabbitmq connection" << LogLevel::Error;
                    }
                    return false;
                }
            }
            else if (cfg->SSLKey != "") {
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

        auto socketStatus = amqp_socket_open_noblock(socket, cfg->Hostname.c_str(), cfg->Port, cfg->Timeout);
        if (socketStatus != AMQP_STATUS_OK) {
            if (!quiet) {
                se->Log() << "Could not connect to rabbitmq: " << socketStatus << LogLevel::Error;
            }
            return false;
        }

        if (!CheckAMQPReply(amqp_login(conn, cfg->Vhost.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                       cfg->Username.c_str(), cfg->Password.c_str()),
                            "amqp_login")) {
            return false;
        }

        amqp_channel_open(conn, 1);
        if (!CheckAMQPReply("Open amqp channel")) {
            return false;
        }

        amqp_exchange_declare(conn, 1, amqp_cstring_bytes(cfg->Exchange.c_str()), amqp_cstring_bytes("direct"), 0,
                              cfg->DurableExchange, 0, 0, amqp_empty_table);
        if (!CheckAMQPReply("Declare amqp exchange")) {
            return false;
        }

        for (auto &queueName : *queueNames) {
            auto queueString = amqp_cstring_bytes(queueName.second.c_str());
            amqp_queue_declare(conn, 1, queueString, 0, cfg->DurableQueues, 0, 0, amqp_empty_table);
            if (!CheckAMQPReply(("Declare amqp queue " + queueName.second).c_str())) {
                return false;
            }
            amqp_queue_bind(conn, 1, queueString, amqp_cstring_bytes(cfg->Exchange.c_str()), queueString,
                            amqp_empty_table);
            if (!CheckAMQPReply(("Bind amqp queue " + queueName.second).c_str())) {
                return false;
            }
        }

        connected = true;
        se->Log() << "Rabbitmq (re)connected" << LogLevel::Info;
        return true;
    }

    void RabbitmqClient::SendMessage(Queue queue, const std::string &message) {
        auto queueName = queueNames->find(queue)->second;
        if (connected || Connect(true)) {
            amqp_bytes_t message_bytes;
            message_bytes.len = message.length();
            char *bytes = nullptr;
            bytes = strdup(message.c_str());
            message_bytes.bytes = reinterpret_cast<void *>(bytes);
            auto pubStatus = amqp_basic_publish(conn, 1, amqp_cstring_bytes(cfg->Exchange.c_str()),
                                                amqp_cstring_bytes(queueName.c_str()), 0, 0, NULL, message_bytes);
            if (pubStatus < 0) {
                connected = false;
                se->Log() << "Could not send message to rabbitmq: " << amqp_error_string2(pubStatus) << LogLevel::Error;
                CloseConnection(true);
            }
        }
    }
} // namespace statusengine
