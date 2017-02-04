#ifndef ACCELEROMETER_INTERFACE_HPP
#define ACCELEROMETER_INTERFACE_HPP

#include "Sensor_structs.hpp"

/**
 * @brief The accelerometer_interface class
 * This interfacece is an abstraction of an accelerometer.
 * This is a virtual function, so that there can exist
 * an implementation for every sensor.
 */
class accelerometer_interface
{
public:
    virtual int activateSensor() = 0;
    virtual int configureSensor() = 0;
    virtual int readValues() = 0;

    accelerometer_val_t getValues();

    ~accelerometer_interface();

protected:
        accelerometer_interface();

        float x_val; /// The latest accelerometer measurement for the x-Axis
        float y_val; /// The latest accelerometer measurement for the y-Axis
        float z_val; /// The latest accelerometer measurement for the z-Axis
};

#endif
