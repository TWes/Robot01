#ifndef MESSAGE_TYPES_HPP
#define MESSAGE_TYPES_HPP

typedef enum
{
	NO_ANSWER = 0,
	ANSWER = 1,
	SET_VARIABLE = 2,
	GET_VARIABLE = 4,
	MOVEMENT_INSTRUCTION = 6	

}Message_types_t;

#endif
