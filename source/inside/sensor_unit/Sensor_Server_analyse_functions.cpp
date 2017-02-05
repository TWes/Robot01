#include "Sensor_Server.hpp"

/**
 * @brief calcMagnetometerOrientation
 * Calculates the orientation on the X-Y-Plane from a given magnetometer measurement.
 * @param magn_meas The measurement.
 * @return The orientation on X-Y-Plane.
 */
float calcMagnetometerOrientation(  magnetometer_val_t magn_meas )
{
	float orientation = 0.0;

	float raw_angle = acos( magn_meas.y_val / \
		 ( sqrt(  magn_meas.x_val* magn_meas.x_val+ magn_meas.y_val* magn_meas.y_val) ));

	if( magn_meas.x_val > 0 )
	{
		orientation = raw_angle;

	}
	else
	{
		orientation = 2*M_PI - raw_angle;
	}

	return orientation;
}
