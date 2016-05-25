#ifndef SENSOR_CONNECTION_HPP
#define SENSOR_CONNECTION_HPP

#include <string>
#include <thread>
#include <QtGui>

#include "../connection_library/connection_library.hpp"
#include "Sensor_structs.hpp"

typedef enum{
    WRITE_POSE
} action_t;


typedef struct {
    int id;
    bool stream;
    action_t todo_action;
    timeval timestamp;
    timeval time_lo_live;
} request_entry_t;


class Sensor_Connection : public socket_inet
{
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


    void start_server();
    void end_server();

    QWidget *parent;


private:
    int actual_id;

    int get_Pose();
};






#endif // SENSOR_CONNECTION_HPP
