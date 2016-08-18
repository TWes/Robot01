#ifndef GPIO_PROTOCOL_HPP
#define GPIO_PROTOCOL_HPP

#define GPIO_Port 2552

typedef enum
{
	NO_ANSWER = 0,	// Client doesnt want anything,
	SET_VARIABLE = 1, // client wants to set a variable on the server
	GET_VARIABLE = 2, // client wants to get a variable from the server
	MOVEMENT_INSTRUCTION = 8, // client sends a movement instruction
	ANSWER = 8
	
}gpio_request_enum_t;

/************
- Operations
*************/

#define SET_IO_PORT 0x01

#define QUIT_SERVER 0x04

#define MOVEMENT_INSTRUCTION 0x01

#define STEER_COMMAND 0x10

/*
1 - stop
2 - foreward
4 - backward
*/


#endif
