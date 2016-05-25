#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<signal.h>

#include "Control_Server.hpp"

Control_Server *Server = NULL;


void sighandler(int signum) { if( Server != NULL && 
                                                       		 signum == SIGINT ) Server->end(); }


int main( int argc, char** argv)
{
	 signal(SIGINT, sighandler);

	Server = new Control_Server( 2554 );

	Server->run();	

	return 0;
}
