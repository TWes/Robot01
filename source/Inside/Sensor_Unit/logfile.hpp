#ifndef LOGFILE_HPP
#define LOGFILE_HPP

#include <string>
#include <fstream>
#include <iostream>

#include <sys/time.h>

class logfile
{
public:
    logfile();
    logfile( std::string filename );
    ~logfile();

    void open();
    void open( std::string filename );
    void close();

    void operator <<( std::string message );

private:
    std::string address;
    std::fstream file;
};

#endif // LOGFILE_HPP
