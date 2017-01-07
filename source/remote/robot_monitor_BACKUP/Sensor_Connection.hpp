#ifndef SENSOR_CONNECTION_HPP
#define SENSOR_CONNECTION_HPP

#include <QtWidgets>
#include <string>
#include <thread>
//#include <QtGui>

#include "socket_inet.hpp"
#include "Sensor_structs.hpp"
#include "sensor_protocol.hpp"
#include "udp_connection_inet.hpp"
#include "graph_helper.hpp"


typedef enum{
    WRITE_POSE,
    PLOT1_IMU,
    PLOT1_FILTERED_VALUES
} action_t;


typedef struct {
    int id;
    bool stream;
    action_t todo_action;
    timeval timestamp;
    timeval time_lo_live;
} udp_request_entry_t;

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

    Position_t act_pose;

    void end();
    bool continue_server;

    std::thread polling_thread;
    void polling_thread_funktion();
    enum{ WAIT_FOR_CONNECTION, CONNECTION_ESTABLISHED} state;

    std::thread receiving_thread;
    void receiving_thread_funktion();
    std::mutex open_requests_mutex;
    std::vector<udp_request_entry_t> open_requests;
    udp_request_entry_t* getEntryById( int id );

    udp_connection *udp_socket;
    udp_connection_information_t udp_socket_information;
    bool imuValuesSubscribed = false;


    void start_server();
    void end_server();

    QWidget *parent;

    int init_UDP_Var(get_variable_enume_t _to_subscribe , action_t _todo, int sending_interval = 1000);
    int unsubscribe_UDP( get_variable_enume_t _to_subscribe  );

signals:
    void debugOutput( QString message );

private:
    int actual_id;

    int get_Pose();
    int init_UDP_Pose();
};

extern Sensor_Connection *sensor_connection;

class GraphHelper;
extern GraphHelper *globalGraphHelper;

#endif // SENSOR_CONNECTION_HPP
