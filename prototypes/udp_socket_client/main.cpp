#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>
#include <string>

int main( int argc, char **argv )
{
    if( argc < 2 )
    {
        std::cout << "Missing Parameter: <ip>" << std::endl;
    }

    std::cout << "ip: " << argv[1] << std::endl;

    int socket_fh = socket( AF_INET, SOCK_DGRAM, 0 );

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons( 6392 );

    inet_aton( argv[1], &server_addr.sin_addr );

    unsigned int addr_len = sizeof( server_addr );

    while(1)
    {
        std::cout << "enter message: " << std::endl;
        std::string message;
        std::cin >> message;

        int send_ret = sendto( socket_fh, message.c_str(), sizeof( message.c_str()),
                               0, (struct sockaddr *) &server_addr, addr_len );

        if( send_ret < 0 )
        {
            std::cout << "Fehler beim senden" << std::endl;
        }

        char buffer[255];
        int rec_ret = recvfrom( socket_fh, buffer, 255, 0,
                                (struct sockaddr *) &server_addr, &addr_len );

        if( rec_ret < 0 )
        {
            std::cout << "Errer receiving data" << std::endl;
        }

        std::cout << "NAchricht: " << buffer << std::endl;


    }

    close( socket_fh );

    return 0;
}
