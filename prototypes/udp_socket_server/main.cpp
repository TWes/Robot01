#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <unistd.h>

int main( int argc, char **argv )
{
    // create a socket
    int socket_fh = socket( AF_INET, SOCK_DGRAM, 0 );
    if( socket_fh < 0 )
    {
        std::cout << "Coulndt create socket" << std::endl;
        return 0;
    }

    struct sockaddr_in own_addr;
    own_addr.sin_family = AF_INET;
    own_addr.sin_port = htons( 0 ); // 6392
    own_addr.sin_addr.s_addr = htonl( INADDR_ANY );

    // bind the socket
    int ret = bind( socket_fh, (struct sockaddr*) &own_addr,
                    sizeof( own_addr ) );
    if( ret < 0 )
    {
        std::cout << "couldn't bind" << std::endl;
    }

    struct sockaddr_in tmp;
    socklen_t tmp_len = sizeof( tmp );

    int info_ret = getsockname( socket_fh, (struct sockaddr*) &tmp, &tmp_len );
    if( info_ret < 0 )
    {
	std::cout << "Error getting socket information" << std::endl;
    }

    std::cout << "Server Port: " << ntohs(tmp.sin_port) << std::endl;

    while(1)
    {
        std::cout << "Waiting for data" << std::endl;

        char buffer[255];
        struct sockaddr_in client_addr;
        unsigned int rec_len = sizeof( client_addr );
        int rec_ret = recvfrom( socket_fh, buffer, 255, 0,
                               (struct sockaddr*) &client_addr,
                                &rec_len );

        if( rec_ret < 0 )
        {
            std::cout << "Konnte nichts empfangen" << std::endl;
        }

        std::cout << "Empfangen von " << inet_ntoa( client_addr.sin_addr )
                     << ":" << ntohs( client_addr.sin_port) << std::endl;
        std::cout << "Data: " << buffer << std::endl;

        int send_ret = sendto( socket_fh, buffer, rec_ret, 0,
                               (struct sockaddr*) &client_addr,
                               rec_len );
        if( send_ret < 0 )
        {
            std::cout << "Konnte nicht senden" << std::endl;
        }

    }

    close( socket_fh );

    return 0;
}
