#ifndef CONNECTION_LIBRARY_HPP
#define CONNECTION_LIBRARY_HPP

// THis library summ up all the shared
// pieces of program used in the whoel project

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

        bool continue_server;

        void polling_function();

        virtual void setup() = 0;
        virtual void handle_connection( int client_handle) = 0;
        virtual void cleanup() = 0;

        void run();
        void end();
    };


/*Socket connection*/

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

    typedef enum
    {
        NO_ANSWER = 0,
        ANSWER = 1,
        SET_VARIABLE = 2,
        GET_VARIABLE = 4,
        MOVEMENT_INSTRUCTION = 6
	

    } Message_types_t;

#endif
