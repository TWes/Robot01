#include "magnetometer_lsm9ds1.hpp"

magnetometer_lsm9ds1::magnetometer_lsm9ds1(i2c_access *i2c_interface) : magnetometer_interface()
{
    this->i2c_bus = i2c_interface;
}

magnetometer_lsm9ds1::~magnetometer_lsm9ds1() {}

int magnetometer_lsm9ds1::activateSensor()
{
    i2c_bus->i2c_write<uint8_t>( 0x1e, 0x22, 0x00  ); //disable sleep mode

    return 0;
}

int magnetometer_lsm9ds1::configureSensor()
{
    int16_t max_magn_val[3];
    int16_t min_magn_val[3];

    // initialise values
    for( int i = 0; i < 3; i++)
    {
        max_magn_val[i] = INT16_MIN;
        min_magn_val[i] = INT16_MAX;
    }

    // Read the values
    const int iterations = 10;
    for( int i = 0; i < iterations; i++ )
    {
        int16_t magn_buffer[3];
        int retry_counter = 0;
        int read_ret = -1;

        do
        {

            read_ret = i2c_bus->i2c_read( 0x1e, 0x28, 6, (char*) magn_buffer );

        } while( read_ret < 0 && ++retry_counter < 3 );

        if( read_ret >= 0 )
        {
            for( int j = 0; j < 3; j++)
            {
                if(  max_magn_val[j] < magn_buffer[j] ) max_magn_val[j] = magn_buffer[j];
                if(  min_magn_val[j] > magn_buffer[j] ) min_magn_val[j] = magn_buffer[j];
            }
        }

        usleep( 20000 );
    }

    // Calculate the mean
    int16_t mean_magn_val[3];
    for( int i = 0; i < 3; i++ )
    {
        mean_magn_val[i] = (max_magn_val[i] >> 1) + (min_magn_val[i] >> 1 );
    }

    this->i2c_bus->i2c_write<int16_t>( 0x1e, 0x05, mean_magn_val[0] );
    this->i2c_bus->i2c_write<int16_t>( 0x1e, 0x07, mean_magn_val[1] );
    this->i2c_bus->i2c_write<int16_t>( 0x1e, 0x09, mean_magn_val[2] );

    return 0;
}

int magnetometer_lsm9ds1::readValues()
{
    int16_t magn_buffer[3];
    int retry_counter = 0;
    int read_ret = -1;

    do
    {
        read_ret = i2c_bus->i2c_read( 0x1e, 0x28, 6, (char*) magn_buffer );

    } while( read_ret < 0 && ++retry_counter < 3 );

    if( read_ret >= 0 )
    {
        this->x_val = (magn_buffer[0] * 0.14)/1000.0;
        this->y_val = (magn_buffer[1] * 0.14)/1000.0;
        this->z_val = (magn_buffer[2] * 0.14)/1000.0;
    }

    return read_ret;
}

