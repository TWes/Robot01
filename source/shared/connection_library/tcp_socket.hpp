#ifndef TCP_SOCKET_HPP
#define TCP_SOCKET_HPP

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
#include <fcntl.h>

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <exception>
#include <sstream>

#include <errno.h>
#include <string.h>

namespace tcp {

class connectionError : std::exception
{
public:
    connectionError( int _errornumber) : errornumber(_errornumber) { }
    const char* what()
    {
        std::stringstream ss;
        ss << "Error sending: " << strerror( this->errornumber );
        return ss.str().c_str();
    }
    int getErrorNumber() { return this->errornumber; }

private:
    int errornumber;
};


class Socket
{
public:

    Socket();
    Socket( std::string socket_address, int port );
    ~Socket();


    int start_connection();
    int start_connection( std::string socket_address, int port);
    int start_connection_with_timeout(int timeout_ms);
    int start_connection_with_timeout( std::string socket_address, int port, int timeout_ms);

    void shutdown_connection();

    int sendData( char *buffer, int length );

    int readData( char* buffer, int length );

    bool is_connected;

    std::string Socket_address;
    int port_nr;
    int socket_fd;
    struct sockaddr_in name;

};

class exceptionMessage : public std::exception
{
private:
    const char* message;
public:
    exceptionMessage( const char* msg ) : std::exception(), message( msg ) {}

    virtual const char* what() const throw()
    { return message;  }
};

class Timeout : public exceptionMessage
{ public: Timeout(const char* msg) : exceptionMessage(msg) {} };

}

#endif
