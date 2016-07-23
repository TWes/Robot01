#ifndef MAGNETOMETER_LSM9DS1_HPP
#define MAGNETOMETER_LSM9DS1_HPP



#include "i2c_access.hpp"
#include "magnetometer_interface.hpp"
#include "lsm9ds1_register.hpp"
#include "time_helper.hpp"

typedef struct{

} magnetometer_config_t;

/**
 * @brief The magnetometer_lsm9ds1 class
 * The implementation of the lsm9ds magnetometer
 */
class magnetometer_lsm9ds1 : public magnetometer_interface
{
public:
    magnetometer_lsm9ds1( i2c_access *i2c_interface, int configMode = 0);
    ~magnetometer_lsm9ds1();

    int activateSensor();
    int configureSensor();
    int readValues();

private:
    i2c_access *i2c_bus;

	int configMode;
	magnetometer_config_t config;
	void configure2D();

	magnetometer_val_t readMeanOverTime( float duration_ms );

	float scale;
};


#endif // MAGNETOMETER_LSM9DS1_HPP
