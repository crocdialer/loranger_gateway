#pragma once

#include <unordered_set>
#include <deque>

#include <bcm2835.h>
#include <RH_RF95.h>

#include <crocore/Application.hpp>
#include <netzer/networking.hpp>
#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_service.hpp>

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
    uint8_t len;
    uint8_t from;
    uint8_t to;
    uint8_t id;
    uint8_t flags;
    int8_t rssi;
};

bool is_checksum_valid(const message_t &msg);

class LorangerGateway : public crocore::Application
{

public:

    explicit LorangerGateway(const crocore::Application::create_info_t &create_info);

private:

    void setup() override;

    void update(double time_delta) override;

    void teardown() override;

    void poll_events() override;

    void add_connection(netzer::ConnectionPtr con);

    void remove_connection(netzer::ConnectionPtr con);

    void process_message(const message_t &msg);

    RH_RF95 m_rf95 = RH_RF95(RF_CS_PIN, RF_IRQ_PIN);

    boost::asio::io_service m_io_service;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_work_guard = 
	   boost::asio::make_work_guard(m_io_service); 
    
    netzer::tcp_server m_tcp_server;

    std::unordered_set<netzer::ConnectionPtr> m_connections;

    std::deque<message_t> m_message_queue;

    std::mutex m_mutex_connection, m_mutex_queue;
};

int main(int argc, char *argv[])
{
    crocore::Application::create_info_t create_info = {};
    create_info.arguments = {argv, argv + argc};
    create_info.num_background_threads = 4;
    auto app = std::make_shared<LorangerGateway>(create_info);
    return app->run();
}
