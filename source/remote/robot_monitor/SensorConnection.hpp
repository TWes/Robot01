#ifndef SENSORCONNECTION_HPP
#define SENSORCONNECTION_HPP

#include <memory>

#include "tcp_socket.hpp"
#include "udp_connection_inet.hpp"

/**
 * @brief The SensorConnection class
 * A Singelton class which provides the sensor connection.
 */
class SensorConnection : protected tcp::Socket, protected udp::Socket
{
public:
    static SensorConnection *getInstance();

    int setupTCPConnection( std::string targetIP, int targetPort, int timeoutMS );

private:
    SensorConnection();
    ~SensorConnection();

    static SensorConnection *instance;

};


#endif
