#include <nlohmann/json.hpp>

#include "LorangerGateway.hpp"
#include "NodeTypes.h"

using namespace crocore;


// Dump a buffer trying to display ASCII or HEX depending on contents
std::string buffer_to_string(const uint8_t buff[], int len)
{
    int i;
    bool ascii = true;
    char buf[1024];
    char *buf_ptr = buf;

    // Check for only printable characters
    for(i = 0; i< len; i++)
    {
        if(buff[i]<32 || buff[i]>127)
        {
            if (buff[i]!=0 || i!=len-1)
            {
                ascii = false; 
                break;
            }
        }
    }

    // now do real display according to buffer type
    // note each char one by one because we're not sure 
    // string will have \0 on the end
    for(int i = 0; i< len; i++)
    {
        if(ascii){ buf_ptr += sprintf(buf_ptr, "%c", buff[i]); } 
        else{ buf_ptr += sprintf(buf_ptr, " %02X", buff[i]); }
    }
    return buf;
}

LorangerGateway::LorangerGateway(int argc, char *argv[]) : crocore::Application(argc, argv)
{

}

void LorangerGateway::setup()
{
    crocore::g_logger.set_severity(Severity::DEBUG);

    m_tcp_server = net::tcp_server(background_queue().io_service(), [this](net::tcp_connection_ptr con){ add_connection(con); });  
    
    m_tcp_server.start_listen(TCP_LISTEN_PORT);

    if(!bcm2835_init()){ throw std::runtime_error("bcm2835_init() Failed"); }
  
    LOG_INFO << format("hello loranger_gateway! -- RF95 CS=GPIO%d, RST=GPIO%d", RF_CS_PIN, RF_RST_PIN);
    
    // reset RFM95 module
    pinMode(RF_RST_PIN, OUTPUT);
    digitalWrite(RF_RST_PIN, LOW );
    bcm2835_delay(150);
    digitalWrite(RF_RST_PIN, HIGH );
    bcm2835_delay(100);
  
    if(!m_rf95.init()){ LOG_ERROR << "RF95 module init failed, not good ..."; }
    else 
    {
        // Defaults after init are 868.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
        // The default transmitter power is 13dBm, using PA_BOOST.
        // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
        // you can set transmitter powers from 5 to 23 dBm:

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

        LOG_INFO << format("init success: NodeID=%d @ %3.2fMHz -- listening ...", RF_NODE_ID, RF_FREQUENCY);
    }
}

void LorangerGateway::update(double time_delta)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex_queue);
        
        while(!m_message_queue.empty())
        {
            message_t msg = std::move(m_message_queue.front());
            m_message_queue.pop_front();
            process_message(msg);
        }
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
    if(m_rf95.available())
    {
        message_t msg = {};

        // construct message object 
        msg.len = sizeof(msg.buf);
        msg.from = m_rf95.headerFrom();
        msg.to = m_rf95.headerTo();
        msg.id = m_rf95.headerId();
        msg.flags = m_rf95.headerFlags();
        msg.rssi = m_rf95.lastRssi();
        
        if(m_rf95.recv(msg.buf, &msg.len)) 
        {
            std::unique_lock<std::mutex> lock(m_mutex_queue);
            m_message_queue.push_back(std::move(msg));
        } 
        else{ LOG_WARNING << "receive failed"; }
    }
} 

void LorangerGateway::add_connection(crocore::ConnectionPtr con)
{
    LOG_DEBUG << "hello " << con->description();  
    
    con->set_disconnect_cb([this](crocore::ConnectionPtr c){ remove_connection(c); });

    // mutex
    std::unique_lock<std::mutex> lock(m_mutex_connection);
    m_connections.insert(con);
}
    
void LorangerGateway::remove_connection(crocore::ConnectionPtr con)
{
    LOG_DEBUG << "bye " << con->description();  

    // mutex
    std::unique_lock<std::mutex> lock(m_mutex_connection);
    m_connections.erase(con);
}


void LorangerGateway::process_message(const message_t &msg)
{
    auto log_str = format("Packet[%02d] #%d => #%d %ddB: %s", msg.len, msg.from, msg.to, msg.rssi, buffer_to_string(msg.buf, msg.len).c_str());
    LOG_DEBUG << log_str; 
    
    if(msg.buf[0] == STRUCT_TYPE_SMART_BULB && msg.len == sizeof(smart_bulb_t))
    {
        smart_bulb_t data = {};
        memcpy(&data, msg.buf, msg.len);
        
        json j =
        {
            {"type", "smart_bulb_3000"},
            {"address", msg.from},
            {"rssi", msg.rssi},
            {"light_sensor", data.light_sensor / (float) 255},
            {"acceleration", data.acceleration / (float) 255},
            {"leds_enabled", data.leds_enabled},
            {"battery", data.battery / (float) 255}
        };
        std::unique_lock<std::mutex> lock(m_mutex_connection);
        for(auto &con : m_connections){ con->write(j.dump() + "\n"); }
    }
}