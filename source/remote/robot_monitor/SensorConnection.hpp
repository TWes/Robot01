#ifndef SENSORCONNECTION_HPP
#define SENSORCONNECTION_HPP

#include "tcp_socket.hpp"
#include "udp_connection_inet.hpp"

class SensorConnection
{
public:
    SensorConnection();
    ~SensorConnection();

    void setupTCPConnection( std::string targetIP, int targetPort );

};


#endif
