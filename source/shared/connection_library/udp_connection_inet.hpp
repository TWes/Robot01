#ifndef INET_UDP_HPP
#define INET_UDP_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <thread>
#include <iostream>

namespace udp
{

typedef struct connection_information
{
    connection_information() {}
    connection_information( char *ip, int port)
    {
        inet_aton( ip, &this->adress );
        this->port_nr = port;
    }

    struct in_addr adress;
    int port_nr;

} connection_information_t;


class Socket
{
public:
    Socket();
    ~Socket();

    int createSocket( int portNr );
    int createSocket( int portNr, connection_information_t &port_info );
    int createSocket( connection_information_t &port_info );

    void start_reveiving();
    void end_receiving();

    int receive( char* buffer, int buffer_size );
    int receive( char* buffer, int buffer_size, connection_information_t &sender );
    int send( const char* message, int message_lenght, connection_information_t receiver);

    virtual void setup();
    virtual void handle_connection( char* message, int message_lenght,
                                    connection_information_t other );
    virtual void cleanup();

private:
    int socket_fh;

    bool continue_thread;
    std::thread *polling_thread;
    void polling_function();

};

}
#endif
