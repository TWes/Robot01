#ifndef SERVER_INET_HPP
#define SERVER_INET_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <vector>
#include <thread>
#include <mutex>
#include <string>

#if defined(LOCAL_SERVER) && defined(INET_SERVER)
#error "Server is defined as local AND network"
#endif

#if !defined(LOCAL_SERVER) && !defined(INET_SERVER)
#error Please define LOCAL_SERVER or INET_SERVER makro
#endif

#ifdef INET_SERVER
typedef struct sockaddr_in sockaddress_t

#define SOCKET_ADDRESS_TYPE  PF_INET
#define SOCKET_PROTOCOLL IPPROTO_TCP

#endif

#ifdef LOCAL_SERVER
typedef struct sockaddr_un sockaddress_t;

#define SOCKET_ADDRESS_TYPE PF_LOCAL
#define SOCKET_PROTOCOLL 0

#endif

#ifdef INET_SERVER
typedef struct{	int socket_fd;   \
				sockaddress_t name; \
				socklen_t name_len; \
		} client_socket_entry;

#endif

#ifdef LOCAL_SERVER
typedef struct{	int socket_fd;   \
				sockaddress_t name; \
				socklen_t name_len; \
		} client_socket_entry;
#endif


	class Server_class
	{
	public:
		

			Server_class(std::string socket_address);
			~Server_class();
		
			std::string socket_address;


		int socket_fh;
		sockaddress_t address;

		std::vector<client_socket_entry> *clients;
		std::mutex clients_mutex;
	
		bool continue_server;
		
		void polling_function();
		virtual void handle_connection( int client_handle) = 0;


		void run();
		void end();
	};


#endif
