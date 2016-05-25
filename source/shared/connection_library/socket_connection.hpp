#ifndef SOCKET_CONNECTION_HPP
#define SOCKET_CONNECTION_HPP

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


class socket_connection
{
public:
    socket_connection( std::string socket_address, int datalength);
    ~socket_connection();

    void start_connection();
    void shutdown_connection();

    void sendData();
    void sendData( char *buffer, int length );

    void readData( char* buffer, int length );

    bool is_connected;
    char *data;
    int nmb_of_data;

    std::string Socket_address;
    int socket_fd;
    struct sockaddr_un name;

};


    class socket_connection3i : public socket_connection
    {
        public:
            socket_connection3i( std::string socket_address);
            ~socket_connection3i();


            void setData(int arg1, int arg2, int arg3);

    };

#endif
