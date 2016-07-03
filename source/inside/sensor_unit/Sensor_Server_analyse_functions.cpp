#include "Sensor_Server.hpp"

float calcMagnetometerOrientation( float x_val, float y_val, float z_val )
{
	float orientation = 0.0;

	std::cout << "Input: " << x_val << " - " << y_val << std::endl;
	
	if( y_val == 0.0 && x_val < 0.0 )
	{
		orientation = M_PI;
	}

	else if( y_val == 0.0 && x_val > 0.0 )
	{
		orientation = 0.0;
	}


	else if( y_val > 0.0 )
	{
		float calc = 0.5 * M_PI;
		calc -= atan( x_val / y_val );
		orientation = calc;	
	}

	else if( y_val < 0.0 )
	{
		float calc = 1.5 * M_PI;
		calc -= atan( x_val / y_val );
		orientation = calc;
	}

	return orientation;

}