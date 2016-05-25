#ifndef SENSOR_CONNECTION_HPP
#define SENSOR_CONNECTION_HPP

#include <string>
#include <thread>


#include "../connection_library/connection_library.hpp"
#include "Sensor_structs.hpp"

class Sensor_Connection : public socket_inet
{
public:
    Sensor_Connection(std::string ip_address, int port);
    ~Sensor_Connection();

    Pose_t act_pose;

    void end();
    bool continue_server;

    std::thread polling_thread;
    void polling_thread_funktion();
    enum{ WAIT_FOR_CONNECTION, CONNECTION_ESTABLISHED} state;

    void start_server();
    void end_server();


private:
    int actual_id;

    int get_Pose(Pose_t *new_pose);
};






#endif // SENSOR_CONNECTION_HPP
