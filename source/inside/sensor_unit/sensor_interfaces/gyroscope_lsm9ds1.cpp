#include "gyroscope_lsm9ds1.hpp"


gysroscope_lsm9ds1::gysroscope_lsm9ds1(i2c_access *i2c_interface, XMLWriter &file, int configMode)
{
    this->i2c_bus = i2c_interface;

    this->configMode = configMode;

    this->gyroscope_entry = &file;

    scale = 8.75/1000.0;
}


gysroscope_lsm9ds1::~gysroscope_lsm9ds1()
{
}


int gysroscope_lsm9ds1::activateSensor()
{
	std::cout << "activate Sensor" << std::endl;
    i2c_bus->i2c_write<uint8_t>( GYROSCOPE_ADDRESS, CTRL_REG1_G, (1<<ODR_G)); //disable sleep mode

	return 0;
}


int gysroscope_lsm9ds1::configureSensor()
{
	std::cout << "configureSensor" << std::endl;

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


int gysroscope_lsm9ds1::readValues()
{
	std::cout << "readValues" << std::endl;

    int16_t gyro_values[3];
    int retry_counter = 0;
    int imu1_read_ret = -1;

    do
    {
        imu1_read_ret = i2c_bus->i2c_read( GYROSCOPE_ADDRESS, OUT_X_G ,6, (char*) gyro_values );

    } while( imu1_read_ret < 0 && ++retry_counter < 3 );

    if( imu1_read_ret > 0 )
    {
        this->x_val = gyro_values[0] * this->scale;
        this->y_val = gyro_values[1] * this->scale;
        this->z_val = gyro_values[2] * this->scale;
    }

    return imu1_read_ret;
}

void gysroscope_lsm9ds1::configure()
{
    std::cout << "configure" << std::endl;
}


void gysroscope_lsm9ds1::loadFromConfigFile()
{
	std::cout << "loadFromConfigFile" << std::endl;
    gyroscope_config_t newConfig;

    if( this->gyroscope_entry->getNode( "Gyroscope") != NULL )
    {

    newConfig.origin_x = this->gyroscope_entry->findAttributeInNodeAsFloat( "Offset", "OffsetX", "Gyroscope");
    newConfig.origin_y = this->gyroscope_entry->findAttributeInNodeAsFloat( "Offset", "OffsetY", "Gyroscope");
    newConfig.origin_z = this->gyroscope_entry->findAttributeInNodeAsFloat( "Offset", "OffsetZ", "Gyroscope");
    }

    this->config = newConfig;
}

void gysroscope_lsm9ds1::writeToConfigFile()
{
	std::cout << "writeToConfigFile" << std::endl;
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
