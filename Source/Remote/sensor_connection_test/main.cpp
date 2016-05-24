#include <iostream>
#include <signal.h>

#include "Sensor_Connection.hpp"

Sensor_Connection *connection = NULL;

void sighandler( int signum )
{
    if( connection != NULL &&
            signum == SIGINT )
    {
        connection->end();
    }
}


int main(int argc, char** argv )
{
    signal( SIGINT, sighandler );

    connection = new Sensor_Connection("192.168.1.103", 2553 );

    connection->start_server();

    while( connection->continue_server )
    {
        std::cout << "( " << connection->act_pose.x_pos << " | "
                     << connection->act_pose.y_pos << " )\n"
                     << connection->act_pose.theta << std::endl;

        usleep( 500000 );
    }

    connection->end_server();

    return 0;
}
