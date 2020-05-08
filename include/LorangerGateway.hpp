#pragma once

#include <unordered_set>
#include <deque>

#include <RH_RF95.h>

#include "crocore/Application.hpp"
#include "crocore/networking.hpp"

#define RF_LED_PIN RPI_V2_GPIO_P1_16 // Led on GPIO23 so P1 connector pin #16
#define RF_CS_PIN  RPI_V2_GPIO_P1_24 // Slave Select on CE0 so P1 connector pin #24
#define RF_IRQ_PIN RPI_V2_GPIO_P1_22 // IRQ on GPIO25 so P1 connector pin #22
#define RF_RST_PIN RPI_V2_GPIO_P1_15 // IRQ on GPIO22 so P1 connector pin #15

// Our RFM95 Configuration
#define RF_FREQUENCY  868.00
#define RF_NODE_ID    1

#define TCP_LISTEN_PORT 4444

struct message_t
{
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = 0;
    uint8_t from = 0;
    uint8_t to = 0;
    uint8_t id = 0;
    uint8_t flags = 0;
    int8_t rssi =0 ;
};

class LorangerGateway : public crocore::Application
{

public:

    explicit LorangerGateway(int argc = 0, char *argv[] = nullptr);

private:

    void setup() override;

    void update(double time_delta) override;

    void teardown() override;

    void poll_events() override;

    void add_connection(crocore::ConnectionPtr con);

    void remove_connection(crocore::ConnectionPtr con);

    void process_message(const message_t &msg);

    RH_RF95 m_rf95 = RH_RF95(RF_CS_PIN, RF_IRQ_PIN);

    crocore::net::tcp_server m_tcp_server;

    std::unordered_set<crocore::ConnectionPtr> m_connections;

    std::deque<message_t> m_message_queue;

    std::mutex m_mutex_connection, m_mutex_queue;
};

int main(int argc, char *argv[])
{
    auto app = std::make_shared<LorangerGateway>(argc, argv);
    return app->run();
}
