#include "accelerometer_interface.hpp"

/**
 * @brief accelerometer_interface::accelerometer_interface
 * The constructor for the interface.
 */
accelerometer_interface::accelerometer_interface(){}

/**
 * @brief accelerometer_interface::~accelerometer_interface
 * The deconstructor of the interface
 */
accelerometer_interface::~accelerometer_interface(){}

/**
 * @brief gyroscope_interface::getValues
 * Returns the the latest succesfull gyroscope measurements
 * @return The latest succesfull gyroscope measurements
 */
accelerometer_val_t accelerometer_interface::getValues()
{
    accelerometer_val_t latest_meas;

    latest_meas.x_val = this->x_val;
    latest_meas.y_val = this->y_val;
    latest_meas.z_val = this->z_val;

    return latest_meas;
}
