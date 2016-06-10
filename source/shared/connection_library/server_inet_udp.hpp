#ifndef SERVER_INET_UDP_HPP
#define SERVER_INET_UDP_HPP

#include <netinet/in.h>

class server_inet_udp
{
public:
    server_inet_udp( int portnr );
    ~server_inet_udp();

    int port;
    int socket_fh;
    struct sockaddr_in address;


    bool continue_server;
    void run();
    void end();


};


#endif
