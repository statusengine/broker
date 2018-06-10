#pragma once

#include "MessageHandler.h"

#include <amqp.h>
#include <amqp_ssl_socket.h>
#include <amqp_tcp_socket.h>

namespace statusengine {
    class RabbitmqConfiguration;
    class RabbitmqClient : public MessageHandler {
      public:
        RabbitmqClient(Statusengine *se, RabbitmqConfiguration *cfg);
        ~RabbitmqClient();

        virtual bool Connect();
        virtual bool Connect(bool quiet);
        virtual void SendMessage(const std::string &queue, const std::string &message);

      private:
        virtual bool CheckAMQPReply(char const *context, bool quiet = false);
        virtual bool CheckAMQPReply(amqp_rpc_reply_t x, char const *context, bool quiet = false);
        virtual bool CloseConnection(bool quiet = false);

        RabbitmqConfiguration *cfg;

        amqp_socket_t *socket;
        amqp_connection_state_t conn;

        bool connected;
    };
} // namespace statusengine

#ifdef WITH_RABBITMQ_CX060
inline amqp_exchange_declare_ok_t *amqp_exchange_declare(amqp_connection_state_t state, amqp_channel_t channel,
                                                         amqp_bytes_t exchange, amqp_bytes_t type,
                                                         amqp_boolean_t passive, amqp_boolean_t durable,
                                                         amqp_boolean_t auto_delete, amqp_boolean_t internal,
                                                         amqp_table_t arguments) {
    return amqp_exchange_declare(state, channel, exchange, type, passive, durable, arguments);
}
#endif // WITH_RABBITMQ_CX060
