#ifndef SENSOR_PROTOCOL_HPP
#define SENSOR_PROTOCOL_HPP


typedef enum{
	NO_ANSWER = 0,	// Client doesnt want anything
	SET_VARIABLE = 1, // client wants to set a variable on the server
	GET_VARIABLE = 2, // client wants to get a variable from the server
	MOVEMENT_INSTRUCTION = 4, // client sends a movement instruction
    SUBSCRIBE_UDP =5, // client subscribe to movement information (pose, velocity)
    UNSUBSCRIBE_UDP=6, // client unsubcribe UDP connection
    LOOPBACK = 7
} sensor_request_enum_t;


typedef enum{
    GET_POSE = 1,
    GET_RAW_IMU_VALUES = 2,
    GET_FILTERED_IMU_VALUES = 3
} get_variable_enume_t;


#endif
