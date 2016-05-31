
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>

#include <vector>
#include <thread>
#include <mutex>
#include <string>

#include "server_inet.hpp"
#include "socket_inet.hpp"

#include "Steering.hpp"

#include "GPIO_Interface.hpp"

#include "gpio_protocol.hpp"

class gpio_Server : public Server_inet
{
public:
	gpio_Server(int port);
	~gpio_Server();

	Steering steer;

    socket_inet *sensor_server_connection;
    void boroadcast_direction_change(uint32_t left_wheel, uint32_t right_wheel);

	void setup();
	void handle_connection( int client_handle);
	void cleanup();
};

void gpio_Server::boroadcast_direction_change(uint32_t left_wheel, uint32_t right_wheel)
{
    if( !this->sensor_server_connection->is_connected )
    {
        // Try to buld up connection
        int ret = this->sensor_server_connection->start_connection();

        if( ret <= 0 )
        {
            std::cout << "Couldnt connect to sensor Server" << std::endl;
        }
    }

    // These are the states we suppose the other nodes know about
    static signed char last_left_wheel_direction = 1;
    static signed char last_right_wheel_direction = 1;

    // See if tere was a change
    bool broadcast_needed = false;
    broadcast_needed = ((left_wheel != last_left_wheel_direction) || (right_wheel != last_right_wheel_direction));

    if( broadcast_needed )
    {
        //Set up the message
        // 0-2 is headder
        // 3-5 is data
        uint16_t message[3+3];

        message[0] = SET_VARIABLE;
        message[1] = 3* sizeof( uint16_t);
        // We dont need an anwer, so id doesnt matter
        message[2] = 42;

        message[3] = 1; // New direction is incomming
        message[4] = left_wheel;
        message[5] = right_wheel;

        int ret = this->sensor_server_connection->sendData( (char*) message, 6* sizeof(uint16_t) );

        if( ret < 0 )
        {
            if( errno == 32) // connection broke down
            {
                this->sensor_server_connection->is_connected = false;
            }
            else
            {
                std::cout << "Error writing to Sensor Server: " << errno << " - "
                          << std::string( strerror(errno) ) << std::endl;
            }
        }
    }

    last_left_wheel_direction = left_wheel;
    last_right_wheel_direction = right_wheel;
}

gpio_Server::gpio_Server(int port) : Server_inet( port )
{
    this->sensor_server_connection = new socket_inet(std::string("127.0.0.1"), 2553 );
}

gpio_Server::~gpio_Server()
{
    delete this->sensor_server_connection;
}

void gpio_Server::setup() 
{    
	this->steer.setup();

    sensor_server_connection->start_connection();

    if( !sensor_server_connection->is_connected )
    {
        std::cout << "Couldnt connect to sensor Server" << std::endl;
    }
    else
    {
        std::cout << "Connect to sensor Server" << std::endl;
    }
}

void gpio_Server::cleanup() 
{
    sensor_server_connection->shutdown_connection();

	this->steer.setdown();
	GPIO_Interface::deleteInstance();
}

void gpio_Server::handle_connection( int client_handle)
{
            uint16_t headder[3];

            // Look if there ist something to read
            int ret = read( client_handle , headder, 3*sizeof(uint16_t));

            if( ret <= 0 )
            {
                 return;
            }


           //std::cout << "Got headder: " << headder[0] << " | Size: " << headder[1]
		// << " | ID: " << headder[2] << std::endl;

            if( headder[0] == MOVEMENT_INSTRUCTION )
            {
                uint16_t data[3];

                // Look if there ist something to read
                int ret = read( client_handle , data, headder[1] );

                if( ret <= 0 )
                {
                    return;
                 }

                //std::cout << "Got a Movement instruction: " << data[0]
                //             << " | " << data[1] << " | " << data[2]  << std::endl;


                if( data[0] == STEER_COMMAND )
                {
                    //std::cout << "Got a steer command: " << data[1] << " | " << data[2] << std::endl;

                    switch( data[1] )
                    {
                        case 1:
                            this->steer.LeftWheelStop();
                            break;

                        case 2:
                            this->steer.LeftWheelForeward();
                            break;

                        case 4:
                            this->steer.LeftWheelBackward();
                            break;

                        default: break;
                    }

                    switch( data[2] )
                    {
                        case 1:
                            this->steer.RightWheelStop();
                            break;

                        case 2:
                            this->steer.RightWheelForeward();
                            break;

                        case 4:
                            this->steer.RightWheelBackward();
                            break;

                        default: break;
                    }

                    this->boroadcast_direction_change(data[1], data[2] );

                } // End of if STEER_COMMAND
            } // End of Movement Instuction

            else
            {
                // Nethertheless read the bytes
                uint8_t data[100];

                std::cout << "Unknown Command: " << headder[0] << ". Read " << headder[1] << " Bytes" << std::endl;

                int ret = read( client_handle , data, headder[1] );
            }


}

gpio_Server *Server = NULL;

void sighandler(int signum) { if( Server!= NULL && 
                            signum == SIGINT ) Server->end();}

int main()
{
	signal(SIGINT, sighandler); 

	Server = new gpio_Server( 2552 );
	
	Server->run();

	delete Server;	

	return 0;
}

