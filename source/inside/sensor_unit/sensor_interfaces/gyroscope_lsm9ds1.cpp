#include "gyroscope_lsm9ds1.hpp"


gyroscope_lsm9ds1::gyroscope_lsm9ds1(i2c_access *i2c_interface, XMLWriter &file, int configMode)
{
    this->i2c_bus = i2c_interface;

    this->configMode = configMode;

    this->gyroscope_entry = &file;

    scale = 8.75/1000.0;
}


gyroscope_lsm9ds1::~gyroscope_lsm9ds1()
{
}


int gyroscope_lsm9ds1::activateSensor()
{

     i2c_bus->i2c_write<uint8_t>( GYROSCOPE_ADDRESS, CTRL_REG1_G, (1<<ODR_G)); //disable sleep mode

	return 0;
}


int gyroscope_lsm9ds1::configureSensor()
{

    switch ( this->configMode )
    {
        case 1:
            //Load from file
            this->loadFromConfigFile();
            break;
        case 2:
            this->configure();
            this->writeToConfigFile();
            break;

        default:
            break;
    }

	return 0;
}


int gyroscope_lsm9ds1::readValues()
{
    int16_t gyro_values[3];
    int retry_counter = 0;
    int imu1_read_ret = -1;

    do
    {
        //imu1_read_ret = i2c_bus->i2c_read( GYROSCOPE_ADDRESS, OUT_X_G ,6, (char*) gyro_values );
	imu1_read_ret = i2c_bus->i2c_read( 0x6b, 0x18, 6, (char*) gyro_values);


    } while( imu1_read_ret < 0 && ++retry_counter < 3 );

    if( imu1_read_ret >= 0 )
    {
	// convert from mdps to gradient per second
	float convert_to_gradient = (2 * M_PI)/(360);

        this->x_val = (gyro_values[0] * this->scale - this->config.origin_x) * convert_to_gradient;
        this->y_val = (gyro_values[1] * this->scale - this->config.origin_y) * convert_to_gradient;
        this->z_val = (gyro_values[2] * this->scale - this->config.origin_z) * convert_to_gradient;
    }

    return imu1_read_ret;
}

void gyroscope_lsm9ds1::configure()
{
	std::cout << "Let the robot stand still for 5 seconds" << std::endl;

	gyroscope_config_t loc_config;

	gyroscope_val_t mean = readMeanOverTime( 5000 );

	loc_config.origin_x = mean.x_val;
	loc_config.origin_y = mean.y_val;
	loc_config.origin_z = mean.z_val;

	std::cout << "x: " << mean.x_val << "; y: " << mean.y_val << "; z: " << mean.z_val << std::endl;

	this->config = loc_config;
}

gyroscope_val_t gyroscope_lsm9ds1::readMeanOverTime( float duration_ms )
{
	gyroscope_val_t ret;

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
		gyroscope_val_t act_vals = this->getValues();

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



void gyroscope_lsm9ds1::loadFromConfigFile()
{
    gyroscope_config_t newConfig;

    if( this->gyroscope_entry->getNode( "Gyroscope") != NULL )
    {

    newConfig.origin_x = this->gyroscope_entry->findAttributeInNodeAsFloat( "Offset", "OffsetX", "Gyroscope");
    newConfig.origin_y = this->gyroscope_entry->findAttributeInNodeAsFloat( "Offset", "OffsetY", "Gyroscope");
    newConfig.origin_z = this->gyroscope_entry->findAttributeInNodeAsFloat( "Offset", "OffsetZ", "Gyroscope");
    }

    this->config = newConfig;
}

void gyroscope_lsm9ds1::writeToConfigFile()
{
    XMLElement *gyroscopeNode = this->gyroscope_entry->getNode( "Gyroscope" );
    if( gyroscopeNode == NULL )
    {
        if( this->gyroscope_entry->getRoot() == NULL )
        {
            std::cout << "Root is zero" << std::endl;
            return;
        }

        gyroscopeNode = this->gyroscope_entry->getRoot()->addElement( "Gyroscope");
        gyroscopeNode->addAttribute( "model", "lsm9ds1");
    }

    std::stringstream ss;

    XMLElement * offsetNode = gyroscopeNode->findNode( "Offset" );
    if( offsetNode == NULL )
    {
        offsetNode = gyroscopeNode->addElement( "Offset" );
    }
    offsetNode->setNoChild();
    ss.str(std::string()); ss << this->config.origin_x;
    offsetNode->addAttribute( "OffsetX", ss.str() );
    ss.str(std::string()); ss << this->config.origin_y;
    offsetNode->addAttribute( "OffsetY", ss.str() );
    ss.str(std::string()); ss << this->config.origin_z;
    offsetNode->addAttribute( "OffsetZ", ss.str() );
}
