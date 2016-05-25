#include "Server_inet.hpp"

Server_inet::Server_inet( int portnr )
{
	clients = new std::vector<client_socket_entry>();
	continue_server = false;
	this->port = portnr;
}

Server_inet::~Server_inet()
{
	delete clients;
	clients = NULL;
}


void Server_inet::polling_function()
{
	this->setup();

	while(this->continue_server)
	{
		if( clients->size() == 0 )
		{
			// if there are no connections, just sleep
			sleep( 1 );
		}


		clients_mutex.lock();

		for( client_socket_entry iter : *clients )
		{	
			 this->handle_connection( iter.socket_fh );
		}

		clients_mutex.unlock();	
	}

	this->cleanup();
}

void Server_inet::run()
{
	// start thread
	this->continue_server = true;

	std::thread polling_thread( &Server_inet::polling_function, this);

	// create socket
	this->socket_fh = socket(  PF_INET, 
							SOCK_STREAM | SOCK_NONBLOCK, 
							IPPROTO_TCP);
		
	if( this->socket_fh < 0 )
	{
		std::cout << "Konnte Socket nicht öffnen" << std::endl;
	}
	
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = htonl( INADDR_ANY );
	this->address.sin_port = htons(  this->port  );
	
	bind( this->socket_fh, (struct sockaddr *) &this->address,
						sizeof(this->address));	

	listen(this->socket_fh, 5);

	do {

		//std::cout << "Waiting for connection" << std::endl;

		client_socket_entry new_connection;
		
		new_connection.name_len = sizeof( struct sockaddr_in );		

		new_connection.socket_fh = accept( this->socket_fh, \
					(struct sockaddr *) &new_connection.name, \
					&new_connection.name_len );

		if( new_connection.socket_fh < 0 )
		{
			if( errno == EAGAIN )
			{
					//std::cout << "Kein neues Gerät" << std::endl; 

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
}

void Server_inet::end()
{
	this->continue_server = false;
}

