#ifndef SENSORCONNECTION_HPP
#define SENSORCONNECTION_HPP

#include <memory>

#include "tcp_socket.hpp"
#include "udp_connection_inet.hpp"
#include "sensor_protocol.hpp"


typedef struct {
    int id;
    bool repeated;
    void (*action)(char*, int);
    timeval timestamp;
    timeval time_lo_live;
} request_entry_t;


/**
 * @brief The SensorConnection class
 * A Singelton class which provides the sensor connection.
 */
class SensorConnection : protected tcp::Socket, protected udp::Socket
{
public:
    static SensorConnection *getInstance();

    // Request
    int actID;
    std::vector<request_entry_t> open_requests;
    request_entry_t getEntryById( int id );


    // TCP related
    int setupTCPConnection( std::string targetIP, int targetPort, int timeoutMS );
    int shutdownTCPConnection();

    // UDP related
    int init_UDP_Var( get_variable_enume_t _to_subscribe , void (*action)(char*, int), int sending_interval, int &id );
    int unsubscribe_UDP( int id );




private:
    SensorConnection();
    ~SensorConnection();

    static SensorConnection *instance;

};


#endif
