#include "connection_library.hpp"

socket_connection::socket_connection( std::string socket_address, int datalength)
{
	this->nmb_of_data = datalength;
	this->data = new char[this->nmb_of_data];
	this->Socket_address = std::string( socket_address);

	is_connected = false;	
}

void socket_connection::start_connection()
{
	// Set up connection
	this->socket_fd = socket( PF_LOCAL, SOCK_STREAM, 0);
	
	this->name.sun_family = AF_LOCAL;
	strcpy( this->name.sun_path, this->Socket_address.c_str() );

	int ret = connect( 	this->socket_fd,
				(struct sockaddr*) &name, 
				SUN_LEN(&name) );
	
	if( ret < 0 )
	{
		std::cout << "Fehler beim Schreiben in der Verbindung. Errornumber: " 
							<< errno << std::endl;

		switch( errno )
		{
		case ENOENT:
			std::cout << "Socketdevice \"" << this->name.sun_path << "\"" 
				  << "existiert nicht! Ist der Server gestartet?\n";
			break;

		case EACCES:
			std::cout << "Keine Rechte um mit Zielsocket zu verbinden.\n";
			break;

		default: break;
		}

		return;
	}

	is_connected = true;
}

void socket_connection::shutdown_connection()
{
	is_connected = false;	
	close( this->socket_fd );
}


void socket_connection::sendData()
{
	if( !is_connected ) return;

	int ret = write( this->socket_fd, this->data, this->nmb_of_data  );

	if( ret < 0 )
	{
		std::cout << "Fehler beim Schreiben in der Verbindung: " 
							<< errno << std::endl;
	}
}

void socket_connection::sendData( char *buffer, int length )
{
	if( !is_connected ) return;

	int ret = write( this->socket_fd, buffer, length  );

	if( ret < 0 )
	{
		std::cout << "Fehler beim Schreiben in der Verbindung: " 
							<< errno << std::endl;
	}
}

void socket_connection::readData( char* buffer, int length )
{
	if( !is_connected ) return;
	
	int ret = read( this->socket_fd, buffer, length );

	if( ret < 0 )
	{
		std::cout << "Fehler beim lesen in der Verbindung: " 
							<< errno << std::endl;
	}
}


socket_connection::~socket_connection()
{
	delete this->data;
}

/*******************************
-- Socket 3i -------------------
*******************************/
socket_connection3i::socket_connection3i( std::string socket_address)
: socket_connection(socket_address, 3)
{
}


socket_connection3i::~socket_connection3i()
{
}

void socket_connection3i::setData(int arg1, int arg2, int arg3)
{
	this->data[0] = arg1;
	this->data[1] = arg2;
	this->data[2] = arg3;
}



