#include <iostream>

#include "linux/joystick.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>

#include <string.h>

#include <fcntl.h>

#include "socket_inet.hpp"
#include "gpio_protocol.hpp"

bool end_program = false;

bool debug_mode = false;

void sighandler(int signum)
{
        if( signum == SIGINT )
            end_program = true;
}


int main( int argc, char** argv )
{
    if( argc < 4 )
    {
        std::cout << "Unsufizient number of arguments: <ip> <port> <joystick> [<options>]" << std::endl;

        return -1;
    }

	for( int i = 4; i < argc; i++ )
	{
		// Enter verbose mode
		if( std::string( argv[i] ).compare( "-v" ) == 0 )
		{
			debug_mode = true;
		}
		else
		{
			std::cout << "Ignoring unknown argument: " << std::string( argv[i] ) << std::endl;
		}
	}

    std::cout << "Programm startet" << std::endl;

    enum{ WAIT_FOR_CONNECTION, CONNECTION_ESTABLISHED} state = WAIT_FOR_CONNECTION;

    uint16_t data[6];

    std::string joystick_name = argv[3];

    socket_inet GPIO_socket( std::string( argv[1]), atoi( argv[2]));

    //open joystick
    int joystick_handle = open( joystick_name.c_str(), O_RDONLY | O_NONBLOCK );

    if( joystick_handle < 0 )
    {
        std::cout << "Konnte Joystick \"" + joystick_name + "\" nicht öffnen.";

        return 1;
    }


    /*  Button Map:
     *
     *  6   -   L1
     *  4   -   L2
     *  7   -   R1
     *  5   -   R2
     */

    uint8_t button_state[4] = {0};


    struct timeval timeout;
    fd_set handles_to_watch;
    FD_ZERO( &handles_to_watch );
    FD_SET( joystick_handle, &handles_to_watch );

    while( !end_program )
    {
        if( state == WAIT_FOR_CONNECTION )
        {
            int ret = GPIO_socket.start_connection();
            if( ret < 0 )
            {
                std::cout << "Couldnt connect so GPIO server: " << errno << " - " 
				<< strerror(errno)  << std::endl;

                sleep( 2 );

                continue;
            }
            else
            {
                std::cout << "Connection established" << std::endl;
                state = CONNECTION_ESTABLISHED;

                data[0] = MOVEMENT_INSTRUCTION;
                data[1] = 3* sizeof( uint16_t );
                data[2] = 12; // Dont care

                data[3] = STEER_COMMAND;
                data[4] = 1;
                data[5] = 1;

                GPIO_socket.sendData( (char*) data, 6*sizeof(uint16_t) );
            }
        }

        else if( state == CONNECTION_ESTABLISHED )
        {

        int ret = select( joystick_handle+1, &handles_to_watch, NULL, NULL, NULL );

        if( ret < 0 )
        {
            std::cout << "Error when calling select" << std::endl;
        }

        js_event joystick_event;

        int bytes_read = read( joystick_handle, &joystick_event, sizeof(js_event) );

        if( bytes_read != sizeof(js_event) )
        { continue; }


        if( joystick_event.type == JS_EVENT_BUTTON )
        {

			if( debug_mode )
			{
				std::cout << "Button " << int( joystick_event.number) << " pressed, got value " <<
                         joystick_event.value << std::endl;
			}
            

            if( button_state[ joystick_event.number - 4 ] != joystick_event.value  )
            {
                button_state[ joystick_event.number - 4 ] = joystick_event.value;

                data[0] = MOVEMENT_INSTRUCTION;
                data[1] = 3* sizeof( uint16_t );
                data[2] = 12; // Dont care
                data[3] = STEER_COMMAND;

                if( button_state[0]) // L2 is pressed
                {
                    data[4] = 4;
                }
                else if( button_state[2] )// L1 is pressed
                {
                    data[4] = 2;
                }
                else
                {
                    data[4] = 1;
                }


                if( button_state[1]) // R2 is pressed
                {
                    data[5] = 4;
                }
                else if( button_state[3] )// R1 is pressed
                {
                    data[5] = 2;
                }
                else
                {
                    data[5] = 1;
                }

                // Send the command
                int ret = GPIO_socket.sendData( (char*) data, 6*sizeof(uint16_t));

                if( ret < 0 )
                {
                    // Socket has been shut down
                    if( errno == 32 )
                    {
                        state = WAIT_FOR_CONNECTION;
                    }

                    else
                    {
                        std::cout << "Fehler: Konnte nicht an GPIO Server senden. Code: "
                                     << errno << std::endl;
                    }

                }
            }
        }

        }// END of CONNECTION Established

    } // ENd of Main while loop

    std::cout << "Close now" << std::endl;


    data[0] = MOVEMENT_INSTRUCTION;
    data[1] = 3* sizeof( uint16_t );
    data[2] = 12; // Dont care

    data[3] = STEER_COMMAND;
    data[4] = 1;
    data[5] = 1;

    GPIO_socket.sendData( (char*) data, 6*sizeof(uint16_t));

    GPIO_socket.shutdown_connection();

    close( joystick_handle );

    return 0;
}
