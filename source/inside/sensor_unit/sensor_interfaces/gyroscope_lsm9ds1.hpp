#ifndef GYROSCOPE_LSM9DS1_HPP
#define GYROSCOPE_LSM9DS1_HPP

#include "i2c_access.hpp"
#include "gyroscope_interface.hpp"
#include "lsm9ds1_register.hpp"
#include "XMLWriter.hpp"
#include "time_helper.hpp"

typedef struct
{
	float origin_x = 0.0;
	float origin_y = 0.0;
	float origin_z = 0.0;

} gyroscope_config_t;

/**
 * @brief The gyroscope_lsm9ds1 class
 * The implementation of the lsm9ds gyroscope
 */
class gyroscope_lsm9ds1 : public gyroscope_interface
{

public:
	gyroscope_lsm9ds1(i2c_access *i2c_interface, XMLWriter &file, int configMode = 0);
    ~gyroscope_lsm9ds1();

    int activateSensor();
    int configureSensor();
    int readValues();

private:
	i2c_access *i2c_bus;
	int configMode;
	gyroscope_config_t config;
	void configure();

	XMLWriter *gyroscope_entry;
    void loadFromConfigFile();
    void writeToConfigFile();

    gyroscope_val_t readMeanOverTime( float duration_ms );

    float scale;

};

#endif
