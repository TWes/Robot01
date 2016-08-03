#include "magnetometer_lsm9ds1.hpp"

magnetometer_lsm9ds1::magnetometer_lsm9ds1(i2c_access *i2c_interface, XMLWriter &file, int configMode )  : magnetometer_interface()
{
    this->i2c_bus = i2c_interface;

    this->configMode = configMode;

    this->magnetometer_entry = &file;

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
            this->loadFromConfigFile();
			break;
		case 2:
			this->configure2D();
            		this->writeToConfigFile();
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
	float newXVal = magn_buffer[0] * scale;
	newXVal = (newXVal - this->config.origin_x) \
			* this->config.scale_x;

	float newYVal = magn_buffer[1] * scale;
	newYVal = (newYVal - this->config.origin_y) \
			* this->config.scale_y;

        this->x_val = newXVal;
        this->y_val = newYVal;
        this->z_val = magn_buffer[2] * scale;
    }

    return read_ret;
}

void magnetometer_lsm9ds1::configure2D()
{
	std::cout << "Config2d" << std::endl;

	magnetometer_config_t newConfig;	

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

	
	std::cout << "Generate configuration." << std::endl;

	// Calc the elipse covering the measurements
	struct{ float x; float y;} NS;
	struct{ float x; float y;} EW;

	NS.x = south.x_val - north.x_val;
	NS.y = south.y_val - north.y_val;
	EW.x = west.x_val - east.x_val;
	EW.y = west.y_val - east.y_val; 	

	float a = 0.5 * sqrt( EW.x * EW.x + EW.y * EW.y  );
	float b = 0.5 * sqrt( NS.x * NS.x + NS.y * NS.y  );
	
	newConfig.scale_x = a/b;

	struct{ float x; float y;} half_NS;
	struct{ float x; float y;} half_EW;
		
	half_NS.x = 0.5 * NS.x;
	half_NS.y = 0.5 * NS.y;
	half_EW.x = 0.5 * EW.x;
	half_EW.y = 0.5 * EW.y;

	struct{ float x; float y;} middle1, middle2;
	middle1.x = north.x_val + half_NS.x;
	middle1.y = north.y_val + half_NS.y;
	middle2.x = east.x_val + half_EW.x;
	middle2.y = east.y_val + half_EW.y;

	newConfig.origin_x = 0.5 * (middle1.x + middle2.x);
	newConfig.origin_y = 0.5 * (middle1.y + middle2.y);

	this->config = newConfig;

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


void magnetometer_lsm9ds1::loadFromConfigFile()
{
    magnetometer_config_t newConfig;
    newConfig.origin_x = this->magnetometer_entry->findAttributeInNodeAsFloat( "Offset", "OffsetX", "Magnetometer");
    newConfig.origin_y = this->magnetometer_entry->findAttributeInNodeAsFloat( "Offset", "OffsetY", "Magnetometer");
    newConfig.origin_z = this->magnetometer_entry->findAttributeInNodeAsFloat( "Offset", "OffsetZ", "Magnetometer");


    newConfig.scale_x = this->magnetometer_entry->findAttributeInNodeAsFloat( "Scale", "ScaleX", "Magnetometer");
    newConfig.scale_y = this->magnetometer_entry->findAttributeInNodeAsFloat( "Scale", "ScaleY", "Magnetometer");
    newConfig.scale_z = this->magnetometer_entry->findAttributeInNodeAsFloat( "Scale", "ScaleZ", "Magnetometer");

    this->config = newConfig;
}

void magnetometer_lsm9ds1::writeToConfigFile()
{
    XMLElement *magnetometerNode = this->magnetometer_entry->getNode( "Magnetometer" );
    if( magnetometerNode == NULL )
    {
	if( this->magnetometer_entry->getRoot() == NULL )
		std::cout << "Root is zero" << std::endl;

        magnetometerNode = this->magnetometer_entry->getRoot()->addElement( "Magnetometer");
	magnetometerNode->addAttribute( "model", "lsm9ds1");
   
    }

    std::stringstream ss;

    XMLElement * scaleNode = magnetometerNode->findNode( "Scale" );
    if( scaleNode == NULL )
    {
	scaleNode = magnetometerNode->addElement( "Scale" );
    }
    scaleNode->setNoChild();
    ss << this->config.scale_x;
    scaleNode->addAttribute( "ScaleX", ss.str() );
    ss.str(std::string()); ss << this->config.scale_y;
    scaleNode->addAttribute( "ScaleY", ss.str() );
    ss.str(std::string()); ss << this->config.scale_z;
    scaleNode->addAttribute( "ScaleZ", ss.str() );

    XMLElement * offsetNode = magnetometerNode->findNode( "Offset" );
    if( offsetNode == NULL )
    {
	offsetNode = magnetometerNode->addElement( "Offset" );
    }
    offsetNode->setNoChild();
    ss.str(std::string()); ss << this->config.origin_x;
    offsetNode->addAttribute( "OffsetX", ss.str() );
    ss.str(std::string()); ss << this->config.origin_y;
    offsetNode->addAttribute( "OffsetY", ss.str() );
    ss.str(std::string()); ss << this->config.origin_z;
    offsetNode->addAttribute( "OffsetZ", ss.str() );
}
