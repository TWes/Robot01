#include "udp_connection_inet.hpp"

udp_connection_inet::udp_connection_inet()
{
    this->polling_thread = NULL;
    this->continue_thread = false;
}

udp_connection_inet::~udp_connection_inet()
{
    if( this->polling_thread != NULL )
    {
        delete this->polling_thread;
    }
}

int udp_connection_inet::createSocket(int portNr)
{
    this->socket_fh = socket( AF_INET, SOCK_DGRAM, 0 );
    if( this->socket_fh < 0 )
    {
        return -errno;
    }

    struct sockaddr_in own_addr;
    own_addr.sin_family = AF_INET;
    own_addr.sin_port = htons( portNr );
    own_addr.sin_addr.s_addr = htonl( INADDR_ANY );

    // bind the socket
    int ret = bind( this->socket_fh, (struct sockaddr*) &own_addr,
                       sizeof( own_addr ) );
    if( ret < 0 )
    {
        return -errno;
    }
}

int udp_connection_inet::createSocket(int portNr, udp_connection_information_t &port_info)
{
    this->socket_fh = socket( AF_INET, SOCK_DGRAM, 0 );
    if( this->socket_fh < 0 )
    {
        return -errno;
    }

    struct sockaddr_in own_addr;
    own_addr.sin_family = AF_INET;
    own_addr.sin_port = htons( portNr );
    own_addr.sin_addr.s_addr = htonl( INADDR_ANY );

    // bind the socket
    int ret = bind( this->socket_fh, (struct sockaddr*) &own_addr,
                       sizeof( own_addr ) );
    if( ret < 0 )
    {
        return -errno;
    }

    // Get own socket informations
    struct sockaddr_in socket_info;
    socklen_t socket_info_len = sizeof( socket_info );

    int info_ret = getsockname( socket_fh, (struct sockaddr*) &socket_info, &socket_info_len );
    if( info_ret < 0 )
    {
        return -errno;
    }

    port_info.adress = socket_info.sin_addr;
    port_info.port_nr = ntohs(socket_info.sin_port);

    return 0;
}

void udp_connection_inet::start_reveiving()
{
    if( this->polling_thread != NULL )
    {
        this->end_receiving();
    }

    this->continue_thread = true;
    this->polling_thread = new std::thread( &udp_connection_inet::polling_function, this);
}

void udp_connection_inet::end_receiving()
{
    this->continue_thread = false;

    if( this->polling_thread != NULL )
    {
        this->polling_thread->join();
        delete this->polling_thread;
        this->polling_thread = NULL;
    }
}

void udp_connection_inet::setup() {}
void udp_connection_inet::cleanup() {}

void udp_connection_inet::polling_function()
{
    this->setup();

    while( this->continue_thread )
    {
        // receive the data
    }

    this->cleanup();
}
