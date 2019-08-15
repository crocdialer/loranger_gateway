#pragma once

#include "crocore/Application.hpp"

#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <RH_RF95.h>


#define RF_LED_PIN RPI_V2_GPIO_P1_16 // Led on GPIO23 so P1 connector pin #16
#define RF_CS_PIN  RPI_V2_GPIO_P1_24 // Slave Select on CE0 so P1 connector pin #24
#define RF_IRQ_PIN RPI_V2_GPIO_P1_22 // IRQ on GPIO25 so P1 connector pin #22
#define RF_RST_PIN RPI_V2_GPIO_P1_15 // IRQ on GPIO22 so P1 connector pin #15

// Our RFM95 Configuration 
#define RF_FREQUENCY  868.00
#define RF_NODE_ID    1

class LorangerGateway : public crocore::Application
{

public:

    explicit LorangerGateway(int argc = 0, char *argv[] = nullptr);

private:

    void setup() override; 

    void update(double time_delta) override;

    void teardown() override;

    void poll_events() override; 

    RH_RF95 m_rf95 = RH_RF95(RF_CS_PIN, RF_IRQ_PIN);
};

int main(int argc, char *argv[])
{
    auto app = std::make_shared<LorangerGateway>(argc, argv);
    return app->run();
}
