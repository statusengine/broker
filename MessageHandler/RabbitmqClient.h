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
        virtual void SendMessage(const std::string &queue, const std::string &message) const;

      private:
        RabbitmqConfiguration *cfg;

        amqp_socket_t *socket;
        amqp_connection_state_t conn;
        amqp_table_t *amqpEmptyTable;
    };
} // namespace statusengine
