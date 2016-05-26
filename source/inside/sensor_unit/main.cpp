#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<signal.h>

#include "Sensor_Server.hpp"

Sensor_Server *Server = NULL;

void sighandler(int signum) { if( Server != NULL && 
                            signum == SIGINT ) Server->end(); }


int main( int argc, char** argv)
{
	 signal(SIGINT, sighandler);

	Server = new Sensor_Server( 2553 );

	Server->run();

	return 0;
}
