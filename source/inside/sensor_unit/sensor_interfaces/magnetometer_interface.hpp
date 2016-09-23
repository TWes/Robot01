#ifndef MAGNETOMETER_INTERFACE_HPP
#define MAGNETOMETER_INTERFACE_HPP

#include "Sensor_structs.hpp"

/**
 * @brief The magnetometer_interface class
 * This interfacece is an abstraction of an magnetometer.
 * This is a virtual function, so that there can exist
 * an implementation for every sensor.
 */
class magnetometer_interface
{

public:
    virtual int activateSensor() = 0;
    virtual int configureSensor() = 0;
    virtual int readValues() = 0;

    magnetometer_val_t getValues();

    ~magnetometer_interface();

protected:
        magnetometer_interface();

        float x_val; /// The latest magnetometer measurement for the x-Axis
        float y_val; /// The latest magnetometer measurement for the y-Axis
        float z_val; /// The latest magnetometer measurement for the z-Axis
};

#endif // MAGNETOMETER_INTERFACE_HPP