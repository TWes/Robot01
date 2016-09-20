#ifndef SENSOR_CONNECTION_HPP
#define SENSOR_CONNECTION_HPP

#include <string>
#include <thread>
#include <QtGui>

#include "socket_inet.hpp"
#include "Sensor_structs.hpp"
#include "sensor_protocol.hpp"
#include "udp_connection_inet.hpp"

typedef enum{
    WRITE_POSE,
    PLOT1,
    PLOT2,
    PLOT3
} action_t;


typedef struct {
    int id;
    bool stream;
    action_t todo_action;
    timeval timestamp;
    timeval time_lo_live;
} request_entry_t;

class Sensor_Connection;

class udp_connection : public udp_connection_inet
{
public:
    udp_connection( Sensor_Connection *sensor );

    void handle_connection(char *message, int message_lenght, udp_connection_information_t other);

private:
    Sensor_Connection *connection;
};


class Sensor_Connection : public QWidget, public socket_inet
{
    Q_OBJECT

public:
    Sensor_Connection(std::string ip_address, int port, QWidget *parent);
    ~Sensor_Connection();

    Pose_t act_pose;

    void end();
    bool continue_server;

    std::thread polling_thread;
    void polling_thread_funktion();
    enum{ WAIT_FOR_CONNECTION, CONNECTION_ESTABLISHED} state;

    std::thread receiving_thread;
    void receiving_thread_funktion();
    std::mutex open_requests_mutex;
    std::vector<request_entry_t> open_requests;
    request_entry_t* getEntryById( int id );


    udp_connection *udp_socket;
    udp_connection_information_t udp_socket_information;

    void start_server();
    void end_server();

    QWidget *parent;

signals:
    void debugOutput( QString message );


private:
    int actual_id;

    int get_Pose();
    int init_UDP_Pose();
    int init_UDP_Var(get_variable_enume_t _to_subscribe , action_t _todo);
};

#endif // SENSOR_CONNECTION_HPP
