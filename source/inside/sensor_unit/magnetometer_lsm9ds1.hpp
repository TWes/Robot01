#ifndef MAGNETOMETER_LSM9DS1_HPP
#define MAGNETOMETER_LSM9DS1_HPP

#include "i2c_access.hpp"
#include "magnetometer_interface.hpp"
#include "lsm9ds1_register.hpp"

/**
 * @brief The magnetometer_lsm9ds1 class
 * The implementation of the lsm9ds magnetometer
 */
class magnetometer_lsm9ds1 : public magnetometer_interface
{
public:
    magnetometer_lsm9ds1( i2c_access *i2c_interface );
    ~magnetometer_lsm9ds1();

    int activateSensor();
    int configureSensor();
    int readValues();

private:
    i2c_access *i2c_bus;

	int16_t x_offset;
	int16_t y_offset;
	int16_t z_offset;
};


#endif // MAGNETOMETER_LSM9DS1_HPP
