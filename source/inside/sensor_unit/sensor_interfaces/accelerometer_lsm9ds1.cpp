#include "accelerometer_lsm9ds1.hpp"


accelerometer_lsm9ds1::accelerometer_lsm9ds1(i2c_access *i2c_interface, XMLWriter &file, int configMode)
{
    this->i2c_bus = i2c_interface;

    this->configMode = configMode;

    this->accelerometer_entry = &file;

    scale = 0.068/1000.0;
}


accelerometer_lsm9ds1::~accelerometer_lsm9ds1()
{
}


int accelerometer_lsm9ds1::activateSensor()
{
    // activate acc
    i2c_bus->i2c_write<uint8_t>( ACCELEROMETER_ADDRESS, CTRL_REG1_G, 0x20  ); //disable sleep mode

	return 0;
}


int accelerometer_lsm9ds1::configureSensor()
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


int accelerometer_lsm9ds1::readValues()
{
    int16_t acc_values[3];
    int retry_counter = 0;
    int imu1_read_ret = -1;

    do
    {
        imu1_read_ret = i2c_bus->i2c_read( ACCELEROMETER_ADDRESS, OUT_X_ACC, 6, (char*) acc_values );

    } while( imu1_read_ret < 0 && ++retry_counter < 3 );

    if( imu1_read_ret >= 0 )
    {
        this->x_val = (acc_values[0] * this->scale - this->config.origin_x);
        this->y_val = (acc_values[1] * this->scale - this->config.origin_y);
        this->z_val = (acc_values[2] * this->scale - this->config.origin_z);
    }

    return imu1_read_ret;
}

void accelerometer_lsm9ds1::configure()
{

	std::cout << "Let the robot stand still for 5 seconds" << std::endl;

	accelerometer_config_t loc_config;

	accelerometer_val_t mean = readMeanOverTime( 5000 );

	loc_config.origin_x = mean.x_val;
	loc_config.origin_y = mean.y_val;
	loc_config.origin_z = mean.z_val;

    this->config = loc_config;
}

accelerometer_val_t accelerometer_lsm9ds1::readMeanOverTime( float duration_ms )
{
	accelerometer_val_t ret;

	// startzeit = Aktuelle Zeit
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
		accelerometer_val_t act_vals = this->getValues();

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

void accelerometer_lsm9ds1::loadFromConfigFile()
{
    accelerometer_config_t newConfig;

    if( this->accelerometer_entry->getNode( "Accelerometer") != NULL )
    {

    newConfig.origin_x = this->accelerometer_entry->findAttributeInNodeAsFloat( "Offset", "OffsetX", "Accelerometer");
    newConfig.origin_y = this->accelerometer_entry->findAttributeInNodeAsFloat( "Offset", "OffsetY", "Accelerometer");
    newConfig.origin_z = this->accelerometer_entry->findAttributeInNodeAsFloat( "Offset", "OffsetZ", "Accelerometer");
    }

    this->config = newConfig;
}

void accelerometer_lsm9ds1::writeToConfigFile()
{
    XMLElement *accelerometerNode = this->accelerometer_entry->getNode( "Accelerometer" );
    if( accelerometerNode == NULL )
    {
        if( this->accelerometer_entry->getRoot() == NULL )
        {
            std::cout << "Root is zero" << std::endl;
            return;
        }

        accelerometerNode = this->accelerometer_entry->getRoot()->addElement( "Accelerometer");
        accelerometerNode->addAttribute( "model", "lsm9ds1");
    }

    std::stringstream ss;

    XMLElement * offsetNode = accelerometerNode->findNode( "Offset" );
    if( offsetNode == NULL )
    {
        offsetNode = accelerometerNode->addElement( "Offset" );
    }
    offsetNode->setNoChild();
    ss.str(std::string()); ss << this->config.origin_x;
    offsetNode->addAttribute( "OffsetX", ss.str() );
    ss.str(std::string()); ss << this->config.origin_y;
    offsetNode->addAttribute( "OffsetY", ss.str() );
    ss.str(std::string()); ss << this->config.origin_z;
    offsetNode->addAttribute( "OffsetZ", ss.str() );
}
