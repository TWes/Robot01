#include "Server_local.hpp"

Server_class::Server_class()
{
	clients = new std::vector<client_socket_entry>();
	continue_server = false;
}

Server_class::~Server_class()
{
	delete clients;
	clients = NULL;
}


void Server_class::polling_function()
{
}

void  Server_class::run()
{
	// create socket
	this->socket_fh = socket( SOCKET_ADDRESS_TYPE,
							SOCK_STREAM | SOCK_NONBLOCK,
							SOCKET_PROTOCOLL );
		
	if( socket_fd < 0 )
	{
		std::cout << "Konnte Socket nicht öffnen" << std::endl;
	}
	
	#ifdef LOCAL_SERVER
		address.sun_family = AF_LOCAL;
		strcpy( address.sun_path, GPIO_ADDRESS );
	#endif


	std::thread polling_thread( &Server_class::polling_function, this);
}

void Server_class::end()
{
	this->continue_server = false;
}








