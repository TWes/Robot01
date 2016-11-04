#include "tcp_socket.hpp"

namespace tcp {

Socket::Socket()
{
    this->Socket_address = "";
    this->port_nr = -1;
}


Socket::Socket(std::string socket_address, int port)
{
    this->Socket_address = socket_address;
    this->port_nr = port;
}

Socket::~Socket()
{

}

int Socket::start_connection()
{
    // Create a socket
    this->socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( this->socket_fd < 0)
    {
        //printf("Failed to create socket\n");
        return -2;
    }

    this->is_connected = true;

    sockaddr_in address_struct;
    address_struct.sin_family = AF_INET;
    address_struct.sin_addr.s_addr = inet_addr( this->Socket_address.c_str() );
    address_struct.sin_port = htons( this->port_nr );

    if (connect( this->socket_fd, (struct sockaddr *) &address_struct, sizeof(address_struct) ) < 0 )
    {
        //printf("Failed to connect with server\n");
        this->is_connected = false;

        close( this->socket_fd );

        return -3;
    }

    return 0;
}

int Socket::start_connection( std::string socket_address, int port)
{
    this->Socket_address = socket_address;
    this->port_nr = port;

    return this->start_connection();
}

void Socket::shutdown_connection()
{
    close( this->socket_fd );
    this->is_connected == false;
}

int Socket::sendData(char *buffer, int length)
{
    if( !this->is_connected ) return 0;

    int ret = send( this->socket_fd, buffer, length, MSG_NOSIGNAL );

    if( ret < 0 )
    {
       // std::cout << "Fehler beim Schreiben in der Verbindung: "
         //                   << errno << std::endl;

        // Connection has been lost
        if( ret == 32 )
        {
            this->is_connected = false;
        }
    }

    return ret;
}

int Socket::readData(char *buffer, int length)
{
    if( !this->is_connected ) return 0;

    int ret = recv( this->socket_fd, buffer, length, MSG_DONTWAIT );

    if( ret < 0 )
    {
        std::cout << "Fehler beim lesen in der Verbindung: "
                            << errno << std::endl;
    }

    return ret;
}

}
