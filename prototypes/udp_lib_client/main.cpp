#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include "udp_connection_inet.hpp"

int main( int argc, char **argv )
{
    if( argc < 3 )
    {
        std::cout << "Missing Parameter: <ip> <port>" << std::endl;
    }

    udp_connection_inet udp_socket;

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

        char buffer[255];

        int rec_ret = udp_socket.receive( buffer, 255 );

        if( rec_ret < 0 )
        {
            std::cout << "Errer receiving data" << std::endl;
        }

        std::cout << "Nachricht: " << buffer << std::endl;
    }

    return 0;
}
