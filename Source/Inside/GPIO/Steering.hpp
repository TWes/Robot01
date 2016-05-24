#ifndef STEERING_HPP
#define STEERING_HPP

#include "../connection_library/connection_library.hpp"

#include "GPIO.hpp"
#include "GPIO_Interface.hpp"

#define LEFT_WHEEL_A_PIN	16
#define LEFT_WHEEL_B_PIN 	19
#define RIGHT_WHEEL_A_PIN 	26
#define RIGHT_WHEEL_B_PIN 	20

class Steering
{
public:
	void setup();
	void setdown();

	void LeftWheelForeward();
	void LeftWheelStop();
	void LeftWheelBackward();
	
	void RightWheelForeward();
	void RightWheelStop();
	void RightWheelBackward();

	Steering();
	~Steering();

};

#endif
