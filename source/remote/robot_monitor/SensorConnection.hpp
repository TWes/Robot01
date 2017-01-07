#ifndef SENSORCONNECTION_HPP
#define SENSORCONNECTION_HPP

#include <memory>
#include <functional>
#include <chrono>
#include <mutex>
#include <vector>
#include <algorithm>
#include <thread>
#include <ctime>

#include "tcp_socket.hpp"
#include "udp_connection_inet.hpp"
#include "sensor_protocol.hpp"
#include "connection_library.hpp"

struct udp_request_entry_t{
    int id;
    bool repeated;
    std::function<void(char*,int)> action;
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::seconds time_lo_live;
};

struct tcp_request_entry_t{
    int id;
    bool repeated;
    std::function<void(void)> onTimeout;
    std::function<void(char*,int)> onReceive;
    std::chrono::system_clock::time_point timestamp;
    std::chrono::milliseconds time_lo_live;
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
    std::vector<struct udp_request_entry_t> openRequests;
    std::mutex openRequestMutex;
    std::vector<udp_request_entry_t> getEntryById( int id );

    // TCP related
    int setupTCPConnection( std::string targetIP, int targetPort, int timeoutMS );
    int shutdownTCPConnection();
    int testTCPConnection(std::function<void(void)> onError, std::function<void(void)> onSuccess );
    std::vector<struct tcp_request_entry_t> openTCPRequests;
    void startPollingThread();
    void endPollingThread();
    bool continueTCPpolling = false;
    std::thread *tcpPollingThread = NULL;
    void tcp_polling_function();
    void checkTCPTimeouts();
    void checkTCPRequest( int id, char* msg, int len );

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
