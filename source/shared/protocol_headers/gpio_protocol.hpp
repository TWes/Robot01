#ifndef GPIO_PROTOCOL_HPP
#define GPIO_PROTOCOL_HPP

#define GPIO_Port 2552

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
