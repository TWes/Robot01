#include "magnetometer_interface.hpp"

/**
 * @brief magnetometer_interface::magnetometer_interface
 * The constructor for the interface.
 */
magnetometer_interface::magnetometer_interface(){}

/**
 * @brief magnetometer_interface::~magnetometer_interface
 * The deconstructor of the interface
 */
magnetometer_interface::~magnetometer_interface(){}

/**
 * @brief magnetometer_interface::getValues
 * Returns the the latest succesfull magnetometer measurements
 * @return The latest succesfull magnetometer measurements
 */
magnetometer_val_t magnetometer_interface::getValues()
{
    magnetometer_val_t latest_meas;

    latest_meas.x_val = this->x_val;
    latest_meas.y_val = this->y_val;
    latest_meas.z_val = this->z_val;

    return latest_meas;
}
