#ifndef CONTROL_SERVER_HPP
#define CONTROL_SERVER_HPP

#include "Server_inet.hpp"

class Control_Server : public Server_inet
{
	public:
		Control_Server(int port);
		~Control_Server();

		void setup();
		void handle_connection( int client_handle );
		void cleanup();
};

#endif 
