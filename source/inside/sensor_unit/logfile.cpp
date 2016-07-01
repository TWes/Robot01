#include "logfile.hpp"

logfile::logfile()
{
    this->address = "";
}

logfile::logfile(std::string filename)
{
    this->address = filename;
}

logfile::~logfile()
{
}

void logfile::open()
{
    this->file.open( this->address );
}

void logfile::open( std::string filename )
{
    this->file.open( filename , std::fstream::out | std::fstream::trunc );

    if( !this->file.is_open() )
    {
        std::cout << "Konnte \"" << filename << "\"nicht öffnen!" << std::endl;
    }

}


void logfile::close()
{

    if( !this->file.is_open() )
    {
        return;
    }

    this->close();
}

void logfile::operator <<(std::string message)
{
    if( !this->file.is_open() )
    {
        return;
    }

    struct timeval tv;
    gettimeofday( &tv, NULL );

    this->file << tv.tv_sec << "." << tv.tv_usec << ": "<< message << std::endl;

    return;
}
