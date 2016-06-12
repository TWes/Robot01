#include "i2c_access.hpp"

i2c_access::i2c_access( )
{
	this->max_buffer_size = 0;
	this->buffer_size = 0;
	this->buffer = NULL;
	this->address_buffer = 0;
}

i2c_access::i2c_access( int max_buffer_size )
{
	this->max_buffer_size = max_buffer_size;
	this->buffer_size = this->max_buffer_size;
	this->buffer = new char[this->max_buffer_size];
	this->address_buffer = 0;
}

i2c_access::~i2c_access()
{
	if( buffer != NULL )
		free( buffer );
}

int i2c_access::open_connection()
{
	this->i2c_fh = open( "/dev/i2c-1", O_RDWR);

	if( this->i2c_fh < 0 )
	{
		std::cerr << "Fehler beim öffnen des i2c Devices" << std::endl;
		std::cerr << "Fehlercode: " << errno << std::endl;
		return errno;
	}

	return 0;
}

void i2c_access::close_connection()
{
	close( this->i2c_fh );
}

int i2c_access::i2c_read( char address, signed int reg, signed int length, char* buffer_out )
{
	if( this->max_buffer_size != 0 &&
			this->max_buffer_size < length )
	{
		/*std::cerr << "Buffer is not big enought to read " << length 
					<<" Bytes, only " << this->max_buffer_size
					<< " are provided" << std::endl; */
		return -1;

	}


	if( this->address_buffer != address ) // if address was not used yet
	{
		if (ioctl( this->i2c_fh, I2C_SLAVE, address) < 0)
		{
			/*std::cerr << "Konnt Addresse " << (int) address_buffer
					<< " nicht öffnen. Fehler: " << errno
					<< ": " << strerror(errno) << std::endl; */
 			return -2;
		}
		else
		{
			this->address_buffer = address;
		}
	}

	// Write designated register to device
	if( write( this->i2c_fh, &reg, 1) != 1 )
	{
       		/*std::cerr << "i2c_access: Konnte Register nicht schreiben: " << errno
			<< ": " << strerror(errno) << std::endl; */
		
		return -3;
   	 }


	// buffer is not big enougth
	if( this->max_buffer_size == 0 &&
 			this->buffer_size < length )
	{
		std::cout << "i2c: Allocate: " << length << " Bytes" << std::endl;

		if( this->buffer != NULL )
		{
			delete this->buffer;		
		}

		this->buffer = new char[ length] ;
		this->buffer_size = length;
	}  


	if( read( this->i2c_fh, this->buffer, length) != length )
	{
		/*std::cout << "i2c_access: Konnte nicht lesen: " << errno
			<< ": " << strerror(errno) << std::endl;*/
		return -4;
	}

	for( int i=0;  i < length; i++ ) 
	{
		buffer_out[i] = this->buffer[i];
	}

	return 0;
}
