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
			this->configure3D();
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
	//std::cout << this->config.origin_x << "; " << this->config.scale_x << std::endl;


	// +- 4 gauss	- 0.14
	// +- 8 gauss	- 0.29	
	float newXVal = magn_buffer[0] * scale;
	newXVal = (newXVal - this->config.origin_x) \
			* this->config.scale_x;

	float newYVal = magn_buffer[1] * scale;
	newYVal = (newYVal - this->config.origin_y) \
			* this->config.scale_y;

	float newZVal = magn_buffer[2] * scale;
	newZVal = (newZVal - this->config.origin_z) \
			* this->config.scale_z;

	this->x_val = newXVal;
        this->y_val = newYVal;
        this->z_val = newZVal;
    }

    return read_ret;
}

void magnetometer_lsm9ds1::configure3D()
{
	std::cout << "Config3d" << std::endl;

	this->config = magnetometer_config_t();

	magnetometer_config_t locConfig;

	std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();
	std::chrono::milliseconds time_since_start(0);
	
	std::vector<float> xVals,yVals,zVals;

	std::cout << "Please rotate the robot around" << std::endl;

	do
	{		
		this->readValues();
		magnetometer_val_t act_vals = this->getValues();
		
		xVals.push_back( act_vals.x_val );
		yVals.push_back( act_vals.y_val );
		zVals.push_back( act_vals.z_val );		

		usleep( 1000 );

		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		time_since_start = std::chrono::duration_cast<std::chrono::milliseconds>( now - startTime);
	
	} while( time_since_start.count() <= 8000 );

	std::cout << "Got values calculating now" << std::endl;

	std::cout << "sizes: " << xVals.size() << "; " << yVals.size() << "; " << zVals.size() << std::endl;

	std::sort( xVals.begin(), xVals.end() );
	float xMax, xMin;
	xMax = *xVals.rbegin();
	xMin = *xVals.begin();

	float xOffset = (xMax + xMin) * 0.5;
	float xScale = 2.0 / (xMax - xMin);

	locConfig.origin_x = xOffset;
	locConfig.scale_x = xScale;

	std::sort( yVals.begin(), yVals.end() );
	float yMax, yMin;
	yMax = *yVals.rbegin();
	yMin = *yVals.begin();

	float yOffset = (yMax + yMin) * 0.5;
	float yScale = 2.0 / (yMax - yMin);

	locConfig.origin_y = yOffset;
	locConfig.scale_y = yScale;

	std::sort( zVals.begin(), zVals.end() );
	float zMax, zMin;
	zMax = *zVals.rbegin();
	zMin = *zVals.begin();

	float zOffset = (zMax + zMin) * 0.5;
	float zScale = 2.0 / (zMax - zMin);

	locConfig.origin_z = zOffset;
	locConfig.scale_z = zScale;

	this->config = locConfig;

	std::cout << "Calibration finished" << std::endl;

	return;
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

	if( this->magnetometer_entry->getNode( "Magnetometer") != NULL )
	{

    newConfig.origin_x = this->magnetometer_entry->findAttributeInNodeAsFloat( "Offset", "OffsetX", "Magnetometer");
    newConfig.origin_y = this->magnetometer_entry->findAttributeInNodeAsFloat( "Offset", "OffsetY", "Magnetometer");
    newConfig.origin_z = this->magnetometer_entry->findAttributeInNodeAsFloat( "Offset", "OffsetZ", "Magnetometer");


    newConfig.scale_x = this->magnetometer_entry->findAttributeInNodeAsFloat( "Scale", "ScaleX", "Magnetometer");
    newConfig.scale_y = this->magnetometer_entry->findAttributeInNodeAsFloat( "Scale", "ScaleY", "Magnetometer");
    newConfig.scale_z = this->magnetometer_entry->findAttributeInNodeAsFloat( "Scale", "ScaleZ", "Magnetometer");
	}

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
