#ifndef GYROSCOPE_INTERFACE_HPP
#define GYROSCOPE_INTERFACE_HPP

#include "Sensor_structs.hpp"

/**
 * @brief The gyroscope_interface class
 * This interfacece is an abstraction of an magnetometer.
 * This is a virtual function, so that there can exist
 * an implementation for every sensor.
 */
class gyroscope_interface
{
public:
    virtual int activateSensor() = 0;
    virtual int configureSensor() = 0;
    virtual int readValues() = 0;

    gyroscope_val_t getValues();

    ~gyroscope_interface();

protected:
        gyroscope_interface();

        float x_val; /// The latest magnetometer measurement for the x-Axis
        float y_val; /// The latest magnetometer measurement for the y-Axis
        float z_val; /// The latest magnetometer measurement for the z-Axis
};

#endif
