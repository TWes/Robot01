#ifndef SENSORCONNECTION_HPP
#define SENSORCONNECTION_HPP

#include <memory>
#include <functional>
#include <chrono>
#include <mutex>
#include <vector>
#include <algorithm>

#include "tcp_socket.hpp"
#include "udp_connection_inet.hpp"
#include "sensor_protocol.hpp"
#include "connection_library.hpp"

struct request_entry_t{
    int id;
    bool repeated;
    std::function<void(char*,int)> action;
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::seconds time_lo_live;
};


/**
 * @brief The SensorConnection class
 * A Singelton class which provides the sensor connection.
 */
class SensorConnection : private tcp::Socket, private udp::Socket
{
public:
    static SensorConnection *getInstance();

    // Request
    int actID;
    std::vector<struct request_entry_t> openRequests;
    std::mutex openRequestMutex;
    std::vector<request_entry_t> getEntryById( int id );

    // TCP related
    int setupTCPConnection( std::string targetIP, int targetPort, int timeoutMS );
    int shutdownTCPConnection();

    // UDP related
    udp::connection_information_t udp_socket_information;
    int init_UDP_Var( get_variable_enume_t _to_subscribe , std::function<void(char*,int)>, int sending_interval, int &id );
    int unsubscribe_UDP( int id );
    void handle_connection( char* message, int message_lenght,
                                    udp::connection_information_t other );



private:
    SensorConnection();
    ~SensorConnection();

    static SensorConnection *instance;

};


#endif
