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

int Socket::start_connection_with_timeout(int timeout_ms)
{
    // Create a socket
    this->socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if( this->socket_fd < 0)
    {
        this->shutdown_connection();
        throw connectionError(errno);
    }

    // Set socket nonblocking
    long arg = fcntl(this->socket_fd, F_GETFL, NULL);
    arg |= O_NONBLOCK;
    fcntl(this->socket_fd, F_SETFL, arg);

    sockaddr_in address_struct;
    address_struct.sin_family = AF_INET;
    address_struct.sin_addr.s_addr = inet_addr( this->Socket_address.c_str() );
    address_struct.sin_port = htons( this->port_nr );

    int ret = connect( this->socket_fd, (struct sockaddr *) &address_struct, sizeof(address_struct) );
    //std::cout << "connect returned: " << ret << "err: " << errno <<  std::endl;

    if( ret < 0 )
    {
        switch( errno )
        {
        case EINPROGRESS:
        {
            struct timeval timeoutStruct;
            timeoutStruct.tv_sec = timeout_ms / 1000;
            timeoutStruct.tv_usec = timeout_ms - (timeoutStruct.tv_sec*1000);

            fd_set socketHandles;
            FD_ZERO(&socketHandles);
            FD_SET(this->socket_fd, &socketHandles );

            int selectRet = select(this->socket_fd+1, NULL, &socketHandles, NULL, &timeoutStruct);

            //std::cout << "selectRet: " << selectRet << " err: " << errno << std::endl;

            if ( selectRet < 0 )
            {
                this->shutdown_connection();
                throw connectionError(errno);
            }
            // No descriptor available => no connection
            else if ( selectRet == 0 )
            {
                this->shutdown_connection();
                throw Timeout("Timeout while connecting to target.");
            }

        } break;

        default:
            throw connectionError( errno );
            break;
        }
    }

    this->is_connected = true;

    return 0;
}

int Socket::start_connection_with_timeout( std::string socket_address, int port, int timeout_ms)
{
    this->Socket_address = socket_address;
    this->port_nr = port;

    return this->start_connection_with_timeout( timeout_ms );
}


void Socket::shutdown_connection()
{
    close( this->socket_fd );
    this->socket_fd = -1;
    this->is_connected = false;
}

int Socket::sendData(char *buffer, int length)
{
    if( !this->is_connected ) return 0;

    int ret = send( this->socket_fd, buffer, length, MSG_NOSIGNAL );

    if( ret <= 0 )
    {
       // std::cout << "Fehler beim Schreiben in der Verbindung: "
         //                   << errno << std::endl;

        // Connection has been lost
        {
            throw connectionError(errno);
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
        //std::cout << "Fehler beim lesen in der Verbindung: "
        //                    << errno << std::endl;
        throw connectionError(errno);
    }

    return ret;
}

}
