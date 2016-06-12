#include "server_inet.hpp"

Server_inet::Server_inet( int portnr )
{
	clients = new std::vector<client_socket_entry>();
    this->continue_server = false;
	this->port = portnr;
}

Server_inet::~Server_inet()
{
	delete clients;
	clients = NULL;
}

void Server_inet::close_client( int fh )
{
	// erase from clients vector and set new max_fh
	int max_fh = -1;
	std::vector<client_socket_entry>::iterator element_to_erase;

        clients_mutex.lock();
	for( auto iter = this->clients->begin();
		iter != this->clients->end(); iter++ )
	{
		int act_fh = (*iter).socket_fh;

		if( act_fh == fh )
		{
			element_to_erase = iter;
		}
		else if( act_fh > max_fh )
		{
			max_fh = act_fh;
		}
	}
	
	this->max_client_fh = max_fh;	
	this->clients->erase(  element_to_erase );

        clients_mutex.unlock();	

	// erase from df_set
	FD_CLR( fh, &client_handles );

	// close socket
	close( fh );	
}


void Server_inet::polling_function()
{

	this->setup();

    while( this->continue_server )
    {
        fd_set clients_to_watch = this->client_handles;

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int ret = select( this->max_client_fh +1, &clients_to_watch, NULL, NULL, &timeout );

        if( ret < 0 )
        {
            std::cout << "Fehler bei select." << std::endl;
            continue;
        }
        else if (ret == 0)
        {
            continue;
        }


	std::vector<int> clients_to_process;

        clients_mutex.lock();
        for( client_socket_entry iter : *clients )
        {
            if( FD_ISSET(iter.socket_fh, &clients_to_watch ) )
            {
		clients_to_process.push_back( iter.socket_fh );
            }
        }
        clients_mutex.unlock();


	for( int fh_to_handle : clients_to_process )
	{
		char buf;
		if( recv( fh_to_handle, &buf, 1, MSG_PEEK) == 0 )
		{
			this->close_client( fh_to_handle );
		}
		else
		{
			this->handle_connection( fh_to_handle );	
		}
		
	}
    }

	this->cleanup();
}

void Server_inet::run()
{
	// start thread
    this->continue_server = true;

    FD_ZERO( &(this->client_handles) );

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


        	if( new_connection.socket_fh > this->max_client_fh )
        	{
	            this->max_client_fh = new_connection.socket_fh;
	        }

        	FD_SET( new_connection.socket_fh, &this->client_handles );

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

