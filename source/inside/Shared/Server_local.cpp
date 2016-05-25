#include "Server_local.hpp"

Server_local::Server_local(std::string socket_address)
{
	clients = new std::vector<client_socket_entry>();
	continue_server = false;
	this->socket_address = socket_address;
}

Server_local::~Server_local()
{
	delete clients;
	clients = NULL;
}


void Server_local::polling_function()
{
	this->setup();

	while(this->continue_server)
	{
		clients_mutex.lock();

		for( client_socket_entry iter : *clients )
		{	
			 this->handle_connection( iter.socket_fh );
		}

		clients_mutex.unlock();
	}

	this->cleanup();
}

void  Server_local::run()
{
	// start thread
	this->continue_server = true;

	std::thread polling_thread( &Server_local::polling_function, this);

	// create socket
	this->socket_fh = socket(  PF_LOCAL,
							SOCK_STREAM | SOCK_NONBLOCK,
							0 );
		
	if( this->socket_fh < 0 )
	{
		std::cout << "Konnte Socket nicht öffnen" << std::endl;
	}
	
	this->address.sun_family = AF_LOCAL;
	strcpy( this->address.sun_path, this->socket_address.c_str() );

	bind( this->socket_fh, (struct sockaddr *) &this->address,
						 SUN_LEN(&this->address));	

	listen(this->socket_fh, 5);

	do {
		client_socket_entry new_connection;
		
		new_connection.name_len = sizeof( struct sockaddr_un );
		socklen_t lenght = sizeof( struct sockaddr_un );

		new_connection.socket_fh = accept( this->socket_fh, \
					(struct sockaddr *) &new_connection.name, \
					&new_connection.name_len );

		if( new_connection.socket_fh < 0 )
		{
			if( errno == EAGAIN )
			{
					sleep(2);
					 continue;
			}	

			std::cout << "Fehler beim Annehmen der Verbindung: " 
							<< errno << std::endl;
			continue;
		}			
		std::cout << "Neue Verbindung"  << std::endl;

		this->clients_mutex.lock();
			this->clients->push_back( new_connection );
		this->clients_mutex.unlock();

	} while( this->continue_server );
	
	std::cout << "Close Server" << std::endl;

	// Thread wird auch beendet
	// deshalb warten wir hier	
	polling_thread.join();

	// Schließe alle Verbindungen
	for( client_socket_entry iter : *clients )
	{
		close( iter.socket_fh );
	}	
	close(this->socket_fh);
	unlink( this->socket_address.c_str() );
}

void Server_local::end()
{
	this->continue_server = false;
}








