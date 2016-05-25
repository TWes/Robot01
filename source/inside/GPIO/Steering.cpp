#include "Steering.hpp"

Steering::Steering()
{}

Steering::~Steering()
{}

void Steering::setup()
{}

void Steering::setdown()
{}

void Steering::LeftWheelForeward()
{
	SetDigital( LEFT_WHEEL_A_PIN, 0);
	SetDigital( LEFT_WHEEL_B_PIN, 1);
}

void Steering::LeftWheelStop()
{
	SetDigital( LEFT_WHEEL_A_PIN, 0);
	SetDigital( LEFT_WHEEL_B_PIN, 0);	
}

void Steering::LeftWheelBackward()
{
	SetDigital( LEFT_WHEEL_A_PIN, 1);
	SetDigital( LEFT_WHEEL_B_PIN, 0);
}
	
void Steering::RightWheelForeward()
{
	SetDigital( RIGHT_WHEEL_A_PIN, 0);
	SetDigital( RIGHT_WHEEL_B_PIN, 1);
}

void Steering::RightWheelStop()
{
	SetDigital( RIGHT_WHEEL_A_PIN, 0);
	SetDigital( RIGHT_WHEEL_B_PIN, 0);
}

void Steering::RightWheelBackward()
{
	SetDigital( RIGHT_WHEEL_A_PIN, 1);
	SetDigital( RIGHT_WHEEL_B_PIN, 0);
}

