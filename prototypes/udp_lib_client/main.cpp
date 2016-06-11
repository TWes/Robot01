#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "udp_connection_inet.hpp"

class udp_connection : public udp_connection_inet
{
    void handle_connection(char *message, int message_lenght,
                           udp_connection_information_t other)
    {
        std::cout << "Nachricht: " << message << std::endl;
    }
};

int main( int argc, char **argv )
{
    if( argc < 3 )
    {
        std::cout << "Missing Parameter: <ip> <port>" << std::endl;

        return -1;
    }

    udp_connection udp_socket;

    udp_socket.start_reveiving();

    // Create the socket
    udp_socket.createSocket(0);

    udp_connection_information_t server( argv[1] , atoi( argv[2]) );

    while(1)
    {
        std::cout << "enter message: " << std::endl;
        std::string message;
        std::cin >> message;

        int send_ret = udp_socket.send( message.c_str(), sizeof(message.c_str()),
                                        server );

        if( send_ret < 0 )
        {
            std::cout << "Fehler beim senden" << std::endl;
        }
    }

    udp_socket.end_receiving();

    return 0;
}
