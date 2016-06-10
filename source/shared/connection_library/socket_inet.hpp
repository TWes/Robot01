#ifndef SOCKET_INET_HPP
#define SOCKET_INET_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/types.h>


#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>

#include <errno.h>

class socket_inet
{
public:
    socket_inet( std::string socket_address, int port);
    ~socket_inet();

    int start_connection();
    void shutdown_connection();

    int sendData( char *buffer, int length );

    int readData( char* buffer, int length );

    bool is_connected;

    std::string Socket_address;
    int port_nr;
    int socket_fd;
    struct sockaddr_in name;

};

#endif
