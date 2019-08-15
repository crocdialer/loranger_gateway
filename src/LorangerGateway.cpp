#include "LorangerGateway.hpp"

using namespace crocore;

LorangerGateway::LorangerGateway(int argc, char *argv[]) : crocore::Application(argc, argv)
{

}

void LorangerGateway::setup()
{
    crocore::g_logger.set_severity(Severity::DEBUG);

    m_tcp_server = net::tcp_server(background_queue().io_service(), [this](net::tcp_connection_ptr con){ add_connection(con); });  
    
    m_tcp_server.start_listen(4444);

    if(!bcm2835_init()){ throw std::runtime_error("bcm2835_init() Failed"); }
  
    printf( "RF95 CS=GPIO%d", RF_CS_PIN);
    printf( ", RST=GPIO%d", RF_RST_PIN );
    
    // reset RFM95 module
    pinMode(RF_RST_PIN, OUTPUT);
    digitalWrite(RF_RST_PIN, LOW );
    bcm2835_delay(150);
    digitalWrite(RF_RST_PIN, HIGH );
    bcm2835_delay(100);
  
    if(!m_rf95.init()){ LOG_ERROR << "RF95 module init failed, not good ..."; }
    else 
    {
        // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

        // The default transmitter power is 13dBm, using PA_BOOST.
        // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
        // you can set transmitter powers from 5 to 23 dBm:
        //  driver.setTxPower(23, false);
        // If you are using Modtronix inAir4 or inAir9,or any other module which uses the
        // transmitter RFO pins and not the PA_BOOST pins
        // then you can configure the power transmitter power for -1 to 14 dBm and with useRFO true. 
        // Failure to do that will result in extremely low transmit powers.
        // rf95.setTxPower(14, true);


        // RF95 Modules don't have RFO pin connected, so just use PA_BOOST
        // check your country max power useable, in EU it's +14dB
        m_rf95.setTxPower(23, false);

        // You can optionally require this module to wait until Channel Activity
        // Detection shows no activity on the channel before transmitting by setting
        // the CAD timeout to non-zero:
        //rf95.setCADTimeout(10000);

        // Adjust Frequency
        m_rf95.setFrequency(RF_FREQUENCY);
        
        // If we need to send something
        m_rf95.setThisAddress(RF_NODE_ID);
        m_rf95.setHeaderFrom(RF_NODE_ID);
        
        // Be sure to grab all node packet 
        // we're sniffing to display, it's a demo
        m_rf95.setPromiscuous(true);

        // We're ready to listen for incoming message
        m_rf95.setModeRx();

        LOG_PRINT << format(" OK NodeID=%d @ %3.2fMHz", RF_NODE_ID, RF_FREQUENCY);
        LOG_PRINT << "listening ...";
    }
}

void LorangerGateway::update(double time_delta)
{

    if (m_rf95.available())
    { 
        // Should be a message for us now
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len  = sizeof(buf);
        uint8_t from = m_rf95.headerFrom();
        uint8_t to   = m_rf95.headerTo();
        //uint8_t id   = m_rf95.headerId();
        //uint8_t flags= m_rf95.headerFlags();
        int8_t rssi  = m_rf95.lastRssi();
        
        if (m_rf95.recv(buf, &len)) 
        {
            printf("Packet[%02d] #%d => #%d %ddB: ", len, from, to, rssi);
            printbuffer(buf, len);
        } 
        else
        {
            //printf("receive failed\n");
            Serial.print("receive failed");
        }
        printf("\n");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void LorangerGateway::teardown()
{
    
    LOG_PRINT << "\nciao " << name();
    bcm2835_close();
}

void LorangerGateway::poll_events()
{

} 


void LorangerGateway::add_connection(crocore::ConnectionPtr con)
{
    LOG_DEBUG << "hello " << con->description();  
    
    con->set_disconnect_cb([this](crocore::ConnectionPtr c){ remove_connection(c); });

    // mutex
    std::unique_lock<std::mutex> lock(m_mutex);
    m_connections.insert(con);
}
    
void LorangerGateway::remove_connection(crocore::ConnectionPtr con)
{
    LOG_DEBUG << "bye " << con->description();  

    // mutex
    std::unique_lock<std::mutex> lock(m_mutex);
    m_connections.erase(con);
}
