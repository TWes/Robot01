#include "magnetometer_lsm9ds1.hpp"

magnetometer_lsm9ds1::magnetometer_lsm9ds1( i2c_access *i2c_interface, int configMode )  : magnetometer_interface()
{
    this->i2c_bus = i2c_interface;

    this->configMode = configMode;

    scale = 0.14/1000.0;
}

magnetometer_lsm9ds1::~magnetometer_lsm9ds1() {}

int magnetometer_lsm9ds1::activateSensor()
{
	char tmp = 0;
	// CTRL Register 1
	tmp = 0;
	tmp |= 0x1 << TEMP_COMP; // Enable temperature compensation
	tmp |= 0x1 << OM; // operation mode medium performance
	tmp |= 0x4 << DO; // Data Rate 10 Hz

	i2c_bus->i2c_write<uint8_t>( MAGNETOMETER_ADDRESS, CTRL_REG1_M, tmp  );

	// CTRL Register 2
	tmp = 0;
	tmp |= 0x0 << FS; // Full-scale +-2 gauss

	i2c_bus->i2c_write<uint8_t>( MAGNETOMETER_ADDRESS, CTRL_REG2_M, tmp  );

	// CTRL Register Registr 3
	tmp = 0;
	tmp |= 0x0 << MD; // Set continuous-conversion mode

	i2c_bus->i2c_write<uint8_t>( MAGNETOMETER_ADDRESS, CTRL_REG3_M, tmp  );

    return 0;
}

int magnetometer_lsm9ds1::configureSensor()
{
	switch ( this->configMode )
	{
		case 1:
			//Load from file
			break;
		case 2:
			this->configure2D();
			break;

		default:
			break;
	}


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
	// +- 4 gauss	- 0.14
	// +- 8 gauss	- 0.29	

        this->x_val = (magn_buffer[0] * 0.14)/1000.0;
        this->y_val = (magn_buffer[1] * 0.14)/1000.0;
        this->z_val = (magn_buffer[2] * 0.14)/1000.0;
    }

    return read_ret;
}

void magnetometer_lsm9ds1::configure2D()
{
	std::cout << "Config2d" << std::endl;
	
	const float duration_ms = 2000.0;

	std::cout << "Point Robot to north [enter]" << std::endl;
	getchar();
	std::cout << "calculating..." << std::endl;


	// Read the north Values
	magnetometer_val_t north = this->readMeanOverTime( duration_ms );

	std::cout << "Point Robot to west [enter]" << std::endl;
	getchar();
	std::cout << "calculating..." << std::endl;

	// Read the west Values	
	magnetometer_val_t west = this->readMeanOverTime( duration_ms );

	
	std::cout << "Point Robot to south [enter]" << std::endl;
	getchar();
	std::cout << "calculating..." << std::endl;

	// Read the south Values
	magnetometer_val_t south = this->readMeanOverTime( duration_ms );

	std::cout << "Point Robot to east [enter]" << std::endl;
	getchar();
	std::cout << "calculating..." << std::endl;

	// Read the east Values
	magnetometer_val_t east = this->readMeanOverTime( duration_ms );

	std::cout << "Calibration finished" << std::endl;
}

magnetometer_val_t magnetometer_lsm9ds1::readMeanOverTime( float duration_ms )
{
	magnetometer_val_t ret;

	// startzeit = Actuelle Zeit
	util::time_t act_time = util::getActTime();
	util::time_t duration = util::calcDuration_ms( duration_ms );
	
	util::time_t start_time, end_time;
	start_time = act_time;
	end_time = act_time + duration;

	float sum[3] = {0.0};
	unsigned int value_count = 0;

	while( act_time < end_time )
	{
		value_count++;

		this->readValues();
		magnetometer_val_t act_vals = this->getValues();		
		
		sum[0] += act_vals.x_val;
		sum[1] += act_vals.y_val;
		sum[2] += act_vals.z_val;

		usleep( 10000 );

		act_time = util::getActTime();
	}

	// berechne durchschnitt
	ret.x_val = sum[0] / value_count;
	ret.y_val = sum[1] / value_count;
	ret.z_val = sum[2] / value_count;	 

	return ret;
}


