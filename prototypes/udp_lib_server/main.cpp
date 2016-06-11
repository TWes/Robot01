#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

#include "udp_connection_inet.hpp"

class udp_connection : public udp_connection_inet
{
    void handle_connection(char *message, int message_lenght,
                           udp_connection_information_t other)
    {

        std::cout << "Empfangen von " << inet_ntoa( other.adress )
                     << ":" << ntohs( other.port_nr ) << std::endl;
        std::cout << "Data: " << message << std::endl;

        int send_ret = this->send( message, message_lenght, other );

        std::cout << "Waiting for data" << std::endl;
    }

    void setup()
    {
        std::cout << "Waiting for data" << std::endl;
    }
};

int main( int argc, char **argv )
{
    // create a socket

    udp_connection udp_socket;

    udp_connection_information_t soket_info;

    udp_socket.createSocket( 0, soket_info );

    std::cout << "Server Port: " << soket_info.port_nr << std::endl;

    udp_socket.start_reveiving();

    while(1) {}

    udp_socket.end_receiving();

    return 0;
}
