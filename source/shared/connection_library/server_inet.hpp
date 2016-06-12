#ifndef SERVER_INET_HPP
#define SERVER_INET_HPP

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

typedef struct{	int socket_fh;   \
                struct sockaddr_in name; \
                socklen_t name_len; \
        } client_socket_entry;


class Server_inet
    {
    public:
        Server_inet( int portnr );
        ~Server_inet();

        int port;
        int socket_fh;
        struct sockaddr_in address;

        std::vector<client_socket_entry> *clients;
        std::mutex clients_mutex;

        fd_set client_handles;
        int max_client_fh;

	void close_client( int fh );

        bool continue_server;

        void polling_function();

        virtual void setup() = 0;
        virtual void handle_connection( int client_handle) = 0;
        virtual void cleanup() = 0;

        void run();
        void end();
    };

#endif
