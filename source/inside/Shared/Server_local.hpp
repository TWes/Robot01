#ifndef SERVER_LOCAL_HPP
#define SERVER_LOCAL_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>


typedef struct{	int socket_fh;   \
				struct sockaddr_un name; \
				socklen_t name_len; \
		} client_socket_entry;


	class Server_local
	{
	public:
		Server_local(std::string socket_address);
		~Server_local();
		
		std::string socket_address;
		int socket_fh;
		struct sockaddr_un address;

		std::vector<client_socket_entry> *clients;
		std::mutex clients_mutex;
	
		bool continue_server;
		
		void polling_function();

		virtual void setup() = 0;
		virtual void handle_connection( int client_handle) = 0;
		virtual void cleanup() = 0;

		void run();
		void end();
	};


#endif
