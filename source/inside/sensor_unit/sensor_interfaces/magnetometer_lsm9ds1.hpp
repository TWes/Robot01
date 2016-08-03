#ifndef MAGNETOMETER_LSM9DS1_HPP
#define MAGNETOMETER_LSM9DS1_HPP

#include "i2c_access.hpp"
#include "magnetometer_interface.hpp"
#include "lsm9ds1_register.hpp"
#include "time_helper.hpp"
#include "XMLWriter.hpp"

typedef struct{

	float scale_x = 1.0;
	float scale_y = 1.0;
	float scale_z = 1.0;

	float origin_x = 0.0;
	float origin_y = 0.0;
	float origin_z = 0.0;

} magnetometer_config_t;

/**
 * @brief The magnetometer_lsm9ds1 class
 * The implementation of the lsm9ds magnetometer
 */
class magnetometer_lsm9ds1 : public magnetometer_interface
{
public:
    magnetometer_lsm9ds1(i2c_access *i2c_interface, XMLWriter &file, int configMode = 0);
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

    XMLWriter *magnetometer_entry;
    void loadFromConfigFile();
    void writeToConfigFile();

	float scale;
};


#endif // MAGNETOMETER_LSM9DS1_HPP
