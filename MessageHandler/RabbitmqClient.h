#pragma once

#include "MessageHandler.h"

#include <amqp.h>
#include <amqp_ssl_socket.h>
#include <amqp_tcp_socket.h>
#include <memory>

namespace statusengine {
    class RabbitmqConfiguration;
    class RabbitmqClient : public MessageHandler {
      public:
        explicit RabbitmqClient(IStatusengine *se, std::shared_ptr<RabbitmqConfiguration> cfg);
        ~RabbitmqClient();

        bool Connect() override;
        bool Connect(bool quiet);
        bool Worker(unsigned long &counter) override;
        void SendMessage(Queue queue, const std::string &message) override;

      private:
        bool CheckAMQPReply(char const *context, bool quiet = false);
        bool CheckAMQPReply(amqp_rpc_reply_t x, char const *context, bool quiet = false);
        bool CloseConnection(bool quiet = false);

        std::shared_ptr<RabbitmqConfiguration> cfg;
        std::shared_ptr<std::map<Queue, std::string>> queueNames;
        std::shared_ptr<std::map<WorkerQueue, std::string>> workerQueueNames;
        std::map<std::string, WorkerQueue> workerQueueNameReverse;

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
