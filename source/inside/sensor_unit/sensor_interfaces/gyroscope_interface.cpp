#include "gyroscope_interface.hpp"

/**
 * @brief gyroscope_interface::magnetometer_interface
 * The constructor for the interface.
 */
gyroscope_interface::gyroscope_interface(){}

/**
 * @brief gyroscope_interface::~gyroscope_interface
 * The deconstructor of the interface
 */
gyroscope_interface::~gyroscope_interface(){}

/**
 * @brief gyroscope_interface::getValues
 * Returns the the latest succesfull gyroscope measurements
 * @return The latest succesfull gyroscope measurements
 */
gyroscope_val_t gyroscope_interface::getValues()
{
    gyroscope_val_t latest_meas;

    latest_meas.x_val = this->x_val;
    latest_meas.y_val = this->y_val;
    latest_meas.z_val = this->z_val;

    return latest_meas;
}
