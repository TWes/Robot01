#include "Sensor_Server.hpp"

float calcMagnetometerOrientation(  magnetometer_val_t magn_meas )
{
	float orientation = 0.0;

	//std::cout << "Input: " << magn_meas.x_val << " - " << magn_meas.y_val
	//		<< " - " <<  magn_meas.z_val << std::endl;
	
	if( magn_meas.y_val == 0.0 && magn_meas.x_val < 0.0 )
	{
		orientation = M_PI;
	}

	else if( magn_meas.y_val == 0.0 && magn_meas.x_val > 0.0 )
	{
		orientation = 0.0;
	}


	else if( magn_meas.y_val > 0.0 )
	{
		float calc = 0.5 * M_PI;
		calc -= atan( magn_meas.x_val / magn_meas.y_val );
		orientation = calc;	
	}

	else if( magn_meas.y_val < 0.0 )
	{
		float calc = 1.5 * M_PI;
		calc -= atan( magn_meas.x_val / magn_meas.y_val );
		orientation = calc;
	}

	//std::cout << "Orientation: " << orientation << std::endl;

	return orientation;

}
