#ifndef I2C_ACCESS
#define I2C_ACCESS

#include <iostream>

#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <errno.h>
#include <string.h>

#include "logfile.hpp"

extern logfile log_file;

class i2c_access
{
public:
	 i2c_access( );
	 i2c_access( int max_buffer_size );
	~ i2c_access();

	int open_connection();
	void close_connection();	

	template <typename T>
	T i2c_read(char address, signed int reg);

	template <typename T>
	void i2c_write(char address, signed int reg, T value);

	void i2c_read( char address, signed int reg, signed int length, char* buffer_out ); 

private: 
	int max_buffer_size;
	int buffer_size;
	char *buffer;
	
	char address_buffer;	

	int i2c_fh;
};

template <typename T> T i2c_access::i2c_read(char address, signed int reg)
{
	T buffer;

	if( this->address_buffer != address ) // if address was not used yet
	{
		if (ioctl( this->i2c_fh, I2C_SLAVE, address) < 0)
		{
			std::cerr << "Konnt Addresse " << (int) address_buffer
					  << " nicht öffnen." << std::endl;	
 			return buffer;
		}
		else
		{
			this->address_buffer = address;
		}
	}

	// Write designated register to device
	if( write( this->i2c_fh, &reg, 1) != 1 )
	{
        std::stringstream message;

         message << "i2c_access: Konnte Register nicht schreiben: "
                  << errno
                  << " - "
                  << std::string( strerror(errno) );

         log_file << message.str();
	}

	if( read( this->i2c_fh, &buffer, sizeof(T) ) != sizeof(T)  )
	{
        std::stringstream message;

        message << std::string( "Fehler beim Lesen: " )
                 << (  errno )
                 << std::string( " - " )
                 << std::string( strerror(errno) ) ;

        log_file << message.str();
	}

	return buffer;
}

template <typename T>
void i2c_access::i2c_write(char address, signed int reg, T value)
{
	char buffer[1+sizeof(T)];

	buffer[0] = reg;

	for( int i=0; i < sizeof(T); i++)
	{
		buffer[i+1] =  value >> ((sizeof(T) - i - 1) *  8);
	}


	if( this->address_buffer != address ) // if address was not used yet
	{
		if (ioctl( this->i2c_fh, I2C_SLAVE, address) < 0)
		{
			std::cerr << "Konnt Addresse " << (int) address_buffer
					  << " nicht öffnen." << std::endl;	
 			return;
		}
		else
		{
			this->address_buffer = address;
		}
	}

	if( write( this->i2c_fh, &buffer, 1+sizeof(T) ) != 1+sizeof(T) )
	{
        std::cerr << "I2C: Konnte nicht schreiben" << std::endl;
	}

	return;
}


#endif
