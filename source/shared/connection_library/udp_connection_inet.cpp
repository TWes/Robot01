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

void udp_connection_inet::handle_connection( char* message, int message_lenght,
                                udp_connection_information_t other ) {}

void udp_connection_inet::polling_function()
{
    this->setup();

    const unsigned int udp_max_buffer_size = 1500;
    char buffer[udp_max_buffer_size];

    while( this->continue_thread )
    {
        fd_set socket_list;
        FD_ZERO( &socket_list );
        FD_SET( this->socket_fh, &socket_list );

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int ret = select( this->socket_fh +1, &socket_list, NULL, NULL, &timeout );

        if( ret < 0 )
        {
            std::cout << "Fehler bei select." << std::endl;
            continue;
        }
        else if (ret == 0)
        {
            continue;
        }

        // receive the data

        struct sockaddr_in client_info;
        socklen_t client_info_len = sizeof( client_info );

        int rec_ret = recvfrom( this->socket_fh, buffer, udp_max_buffer_size,
                                0, (struct sockaddr*)  &client_info, &client_info_len );

        udp_connection_information_t client_info_for_function;
        client_info_for_function.adress = client_info.sin_addr;
        client_info_for_function.port_nr = ntohs(client_info.sin_port);

        this->handle_connection( buffer, rec_ret, client_info_for_function);
    }

    this->cleanup();
}

int udp_connection_inet::send(const char *message, int message_lenght,
                               udp_connection_information_t receiver)
{
    struct sockaddr_in receiver_address;
    receiver_address.sin_family = AF_INET;
    receiver_address.sin_port = htons( receiver.port_nr ) ;
    receiver_address.sin_addr = receiver.adress;

    int send_ret = sendto( this->socket_fh, message, message_lenght,
                           0, (struct sockaddr *) &receiver_address,
                           sizeof(receiver_address) );

    return send_ret;
}

int udp_connection_inet::receive( char* buffer, int buffer_size )
{
    struct sockaddr_in client_info;
    socklen_t client_info_len = sizeof( client_info );

    int rec_ret = recvfrom( this->socket_fh, buffer, buffer_size,
                            0, (struct sockaddr*)  &client_info, &client_info_len );

    return rec_ret;
}

int udp_connection_inet::receive( char* buffer, int buffer_size,
                                  udp_connection_information_t &sender )
{
    struct sockaddr_in client_info;
    socklen_t client_info_len = sizeof( client_info );

    int rec_ret = recvfrom( this->socket_fh, buffer, buffer_size,
                            0, (struct sockaddr*)  &client_info, &client_info_len );

    sender.adress = client_info.sin_addr;
    sender.port_nr = ntohs(client_info.sin_port);

    return rec_ret;
}


udp_connection_information::udp_connection_information(char *ip, int port)
{
    inet_aton( ip, &this->adress );

    this->port_nr = port;
}

udp_connection_information::udp_connection_information() {}
