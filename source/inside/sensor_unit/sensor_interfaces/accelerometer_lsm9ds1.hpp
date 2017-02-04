#ifndef ACCELEROMETER_LSM9DS1_HPP
#define ACCELEROMETER_LSM9DS1_HPP

#include "i2c_access.hpp"
#include "accelerometer_interface.hpp"
#include "lsm9ds1_register.hpp"
#include "XMLWriter.hpp"
#include "time_helper.hpp"

typedef struct
{
	float origin_x = 0.0;
	float origin_y = 0.0;
	float origin_z = 0.0;

} accelerometer_config_t;

/**
 * @brief The accelerometer_lsm9ds1 class
 * The implementation of the lsm9ds accelerometer
 */
class accelerometer_lsm9ds1 : public accelerometer_interface
{

public:
	accelerometer_lsm9ds1(i2c_access *i2c_interface, XMLWriter &file, int configMode = 0);
    ~accelerometer_lsm9ds1();

    int activateSensor();
    int configureSensor();
    int readValues();

private:
	i2c_access *i2c_bus;
	int configMode;
	accelerometer_config_t config;
	void configure();

	XMLWriter *accelerometer_entry;
    void loadFromConfigFile();
    void writeToConfigFile();

    accelerometer_val_t readMeanOverTime( float duration_ms );

    float scale;

};

#endif
