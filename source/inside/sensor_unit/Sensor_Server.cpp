#include "Sensor_Server.hpp"

#include <fstream>
#include <bitset>

logfile log_file;

/**
 * @brief toBinaryString - Converts number to binary string
 * @param number - number which should be converted
 * @return string containing the binary representation of number
 */
std::string toBinaryString( int16_t number )
{
	std::string ret;

	for( int i = 15; i>=0; i--)
	{
		if( number & (1<<i) )
		{
			ret += "1";
		}
		else
		{
			ret += "0";
		}
	}

	return ret;
}

Sensor_Server::Sensor_Server( int argc, char** argv, int portnr ) \
	: Server_inet( portnr )
{
	this->start_up =true;

	this->udp_sending_thread = NULL;

    this->magnetometer = NULL;

	this->evaluate_options( argc, argv );
}

Sensor_Server::~Sensor_Server()
{
	if( this->udp_sending_thread != NULL )
	{
		delete this->udp_sending_thread;
	}

    if( this->magnetometer != NULL )
    {
        delete this->magnetometer;
    }
}

void Sensor_Server::setup()
{
	// If just the help should be shown -> exit
	if( this->options.show_help )
	{
		continue_server = false;
		printHelp();
	}


    log_file.open( "logfile");

    if( !this->options.dummy_mode )
    {
        this->cam_state = 0;

        CAM_thread = std::thread( &Sensor_Server::CAM_thread_funktion, this);

        XMLWriter xmlConfig;
        xmlConfig.addRoot("SensorServerConfig");

        xmlConfig.ReadFromFile( "SensorConfigFile.xml");

        i2c_bus.open_connection();

        //activate magnetometer
        this->magnetometer = new magnetometer_lsm9ds1( &i2c_bus,
                                xmlConfig,
                                (this->options.calibrate_magnetomer? 2 : 1) );
        this->magnetometer->activateSensor();
        this->magnetometer->configureSensor();

        //activate gyroscope
        this->gyroscope = new gysroscope_lsm9ds1( &i2c_bus,
                                                  xmlConfig,
                                                  (this->options.calibrate_gyroscope? 2 : 1));
        this->gyroscope->activateSensor();
        this->gyroscope->configureSensor();

        I2C_thread = std::thread( &Sensor_Server::I2C_thread_funktion, this);

        // activate acc
        i2c_bus.i2c_write<uint8_t>( 0x6B, 0x10, 0x20  ); //disable sleep mode

        working_thread = std::thread( &Sensor_Server::working_thread_function, this );

        xmlConfig.WriteToFile("SensorConfigFile.xml");
    }
    else
    {
        working_thread = std::thread( &Sensor_Server::dummy_thread_function, this );
    }



	// Create the udp socket, port doesn't matter
	udp_connection.createSocket(0);

	this->udp_sending_thread = new std::thread( &Sensor_Server::udp_sending_function, this);


    log_file << "Sensor_server setup completed.";

}

void Sensor_Server::cleanup()
{
	std::cout << "Wait for threads to end" << std::endl;

	webcam.release();

	working_thread.join();

	CAM_thread.join();
	I2C_thread.join();

    	i2c_bus.close_connection();

	log_file << "Sensor Server cleanup compleded (closing logfile now)";

	log_file.close();
}

void Sensor_Server::handle_connection( int client_handle )
{
    int16_t headder[3];

	int ret = read( client_handle, headder, 3*sizeof(int16_t) );
	if( ret <= 0 )
	{
		sleep(1);
		return;
	}

	//std::cout << "Headder: " << headder[0] << " size: " 
	//	<< headder[1] << " id: " << headder[2] << std::endl;

	if( headder[0] == SET_VARIABLE )
    {
        uint16_t data;

        // Look if there ist something to read
        int ret = read( client_handle , &data, sizeof(uint16_t) );

        if( ret <= 0 )
        {
            return;
        }

        // Look what variable should be read
        switch( data )
        {
        case 1: // Got a new direction Read 2 uint16_t
            uint16_t new_value[2];
            ret = read( client_handle , &new_value, 2*sizeof(uint16_t) );

            if( ret <= 0 )
            {
                return;
            }

            switch( new_value[0])
            {
            case 1: right_direction = 0; break;
            case 2: right_direction = 1; break;
            case 4: right_direction = -1; break;
            default: break;
            }

            switch( new_value[1])
            {
            case 1: left_direction = 0; break;
            case 2: left_direction = 1; break;
            case 4: left_direction = -1; break;
            default: break;
            }

            break;

        default:
            break;
        }

	}

	// GET VARIABLE
	else if( headder[0] == GET_VARIABLE )
	{		
		uint32_t buffer;
        ret = read( client_handle , &buffer, headder[1] );

        if( ret <= 0 )
        {
            return;
		}

		// look, which variable should be read
		if( buffer == GET_POSE )
		{
			int16_t answer_header[3];
			answer_header[0] = headder[0];
			answer_header[2] = headder[2];

			//std::cout << "get Pose" << std::endl;
			Position_t pose_buffer = this->act_position;
			answer_header[1] = sizeof(pose_buffer);

			char message[ 3*sizeof(uint16_t) + 1 * sizeof(Position_t ) ];
			memcpy( message, answer_header, 3*sizeof(uint16_t) );
            memcpy( (message + 3*sizeof(uint16_t) ), &pose_buffer, 1 * sizeof(Position_t ) );

			ret = write( client_handle, &message, sizeof(message) );
		}
	}

    else if( headder[0] == SUBSCRIBE_UDP )
	{
		//std::cout << "Subscribed to udp" << std::endl;
		uint32_t data[3];
		ret = read( client_handle , &data, headder[1] );

        if( ret <= 0 )
        {
            return;
		}

		struct sockaddr_in adress = getSocketAdressByFh( client_handle );
		std::cout << "Client: " << std::string(inet_ntoa( adress.sin_addr )) << " and port " << data[1]
		<< " im intervall von " << data[2] << "ms; should send: " << data[0] << std::endl;
		
        udp::connection_information_t client( inet_ntoa( adress.sin_addr ),
				 (int) data[1] );

		UDP_subscriber_entry_t new_entry;
		new_entry.next_sending_time = -1.0;
		new_entry.sending_interval = data[2];
		new_entry.sending_object = data[0];
		new_entry.client_info = client;
		new_entry.seq_number = headder[2];

		this->UDP_subscriber.push_back(new_entry);		
	}
	else if( headder[0] == 6 )
	{
        // Delete the given id
        this->deleteIdInUDPSubscriber(headder[2]);
	}
    else if( headder[0] == LOOPBACK )
    {
        char* buffer = new char[headder[1]];

        ret = read( client_handle, buffer, headder[1] );

        char* msgBuffer = new char[headder[1]+ 3*sizeof(int16_t)];
        memcpy( msgBuffer, headder, 3*sizeof(int16_t) );
        memcpy( msgBuffer + 3*sizeof(int16_t), buffer, headder[1] );

        int sendRet = write( client_handle, msgBuffer, headder[1]+ 3*sizeof(int16_t) );

        std::cout << "Loopback: \"" << buffer << "\" with ret value: " << sendRet << std::endl;

        delete msgBuffer;
        delete buffer;
    }
    else
    {
	std::cout << "Headder: " << headder[0] << " size: " 
		<< headder[1] << " id: " << headder[2] << std::endl;

        // Nethertheless read the bytes
        uint8_t data[100];

        std::cout << "Unknown Command. Read " << headder[1] << " Bytes" << std::endl;

        int ret = write( client_handle , data, headder[1] );
    }



    	if( headder[0] == 1222 )// Outcommented
	{
		// Should Send image

		// Somebody asked for an image
		int32_t image_headder[5];
		
		cv::Mat buffer_image;	

		cam_queue_mutex.lock();
			cam_images.back().image.copyTo( buffer_image );
		cam_queue_mutex.unlock(); 

		//cvtColor(  buffer_image,  buffer_image, CV_RGB2GRAY);

		image_headder[0] = buffer_image.rows;
		image_headder[1] = buffer_image.cols;
		image_headder[2] = buffer_image.channels();
		image_headder[3] = buffer_image.depth();
		image_headder[4] = buffer_image.type();

		int written = write( client_handle, image_headder, 5*sizeof(int32_t) );
		
		int bytes_per_pixel = 1;
		if( buffer_image.depth() == CV_8U || buffer_image.depth() == CV_8S )
		{
			bytes_per_pixel = 1;
		}
		else if( buffer_image.depth() == CV_16U || buffer_image.depth() == CV_16S )
		{
			bytes_per_pixel = 2;
		}
		else if( buffer_image.depth() == CV_32S || buffer_image.depth() == CV_32F )
		{	
			bytes_per_pixel = 4;
		}
		else if( buffer_image.depth() == CV_64F )
		{
			bytes_per_pixel = 8;
		}	

		for( int row = 0 ; row < buffer_image.rows; row++ )
		{
			int writ =  write( client_handle, 
						buffer_image.ptr( row ),
						buffer_image.cols * buffer_image.channels() * bytes_per_pixel );
		}
	}
}

void Sensor_Server::udp_sending_function()
{

	while( this->continue_server )
	{
		// get actual time in ms
		struct timeval act_time_struct;
		gettimeofday( &act_time_struct, NULL);
		float act_time = act_time_struct.tv_sec * 1000.0 +
				act_time_struct.tv_usec / 1000.0;

		//std::cout << "udp sending: " << act_time << std::endl;

		for( UDP_subscriber_entry_t &entry : UDP_subscriber )
		{
			// Do we have to send already?
			if( act_time >= entry.next_sending_time )
			{
				//std::cout << "Should send UDP: " << entry.seq_number << std::endl;		

				switch( entry.sending_object )
				{
				// Send the position
				case GET_POSE:
				{
					//std::cout << "get Pose" << std::endl;
					int16_t answer_header[3];
					answer_header[0] = SUBSCRIBE_UDP;
					answer_header[2] = entry.seq_number;

					Position_t pose_buffer = this->act_position;
					answer_header[1] = sizeof(pose_buffer);

					char message[ 3*sizeof(uint16_t) + 1 * sizeof(Position_t ) ];
					memcpy( message, answer_header, 3*sizeof(uint16_t) );
					memcpy( (message + 3*sizeof(uint16_t) ), &pose_buffer, 1 * sizeof(Position_t ) );  

					this->udp_connection.send( message, sizeof(message),
								entry.client_info);
					}
					break;
	                case GET_RAW_IMU_VALUES:
				{
					int16_t answer_header[3];
					answer_header[0] = SUBSCRIBE_UDP;
					answer_header[2] = entry.seq_number;

					IMU_Measurement act_imu_meas;

					IMU_queue_mutex.lock();
				            act_imu_meas = *(IMU_values.begin());
					IMU_queue_mutex.unlock();

                			answer_header[1] = sizeof( IMU_Measurement );

		                        char message[ 3*sizeof(uint16_t) + sizeof(IMU_Measurement) ];
					memcpy( message, answer_header, 3*sizeof(uint16_t) );
			                memcpy( (message + 3*sizeof(uint16_t) ), &act_imu_meas, sizeof( IMU_Measurement ) );

					this->udp_connection.send( message, sizeof(message),
								entry.client_info);
				}
					break;
				case GET_FILTERED_IMU_VALUES:
					{
					//std::cout << "Send: " << act_status_tuple.magnAngVelZ << std::endl;

					int16_t answer_header[3];
					answer_header[0] = SUBSCRIBE_UDP;
					answer_header[2] = entry.seq_number;

                    Status_tuple_t status_tuple = act_status_tuple;
                            answer_header[1] = sizeof( Status_tuple_t );
		
                                char message[ 3*sizeof(uint16_t) + sizeof( Status_tuple_t ) ];
					memcpy( message, answer_header, 3*sizeof(uint16_t) );
                            memcpy( (message + 3*sizeof(uint16_t) ), &status_tuple, sizeof( Status_tuple_t ) );

					this->udp_connection.send( message, sizeof(message),
								entry.client_info);

					}
					break;
				default:
					std::cout << "should send: " << entry.sending_object << std::endl;
					break;
				}



				// update nex timestamp
				entry.next_sending_time = act_time + entry.sending_interval;
			}
		}


		usleep( 10000 );
	}
	

}

void Sensor_Server::deleteIdInUDPSubscriber(int id)
{
    //std::cout << "Unsubscribe UDP with id: " << id << std::endl;
    //std::cout << "Size before erasing: " << this->UDP_subscriber.size() << std::endl;
    this->UDP_subscriber.erase(
                std::remove_if( this->UDP_subscriber.begin(), this->UDP_subscriber.end() ,
                                [id](UDP_subscriber_entry_t &element)
                                {
                                    return element.seq_number == id;
                                } ) );
    //std::cout << "Size after erasing: " << this->UDP_subscriber.size() << std::endl;
}


void Sensor_Server::CAM_thread_funktion()
{
	while( this->continue_server )
	{
		// Cam is disconnected
		if( this->cam_state == 0)
		{
			static int last_cam_opened = 0
;
			webcam = cv::VideoCapture(  last_cam_opened );

			if( !webcam.isOpened() )
			{
                //std::cout << "No camera detected under nmb: " << last_cam_opened << std::endl;

				last_cam_opened = ((last_cam_opened+1)%10);
				
				// If we're trying the first video again, sleep a little bit
				// so the cpu isnt busy all the time		
				if( last_cam_opened == 0 )
				{
					sleep(2);
				}
			}
			else
			{
				this->cam_state = 1;
			}
		}

		// Cam is connected
		else if( this->cam_state == 1 )
		{
			CAM_image image;

			webcam >> image.image;
		
			if( image.image.data == NULL )
			{
				std::cout << "No image available" << std::endl;
				continue;
			}

			cvtColor(image.image, image.image, CV_BGR2GRAY );
			
			resize( image.image, image.image, cv::Size( 250, 187 )  );

			image.timestamp = std::chrono::system_clock::now();

			cam_queue_mutex.lock();
				cam_images.push( image );
				while( cam_images.size() > 5 )
				{
					cam_images.pop();
				}
			cam_queue_mutex.unlock();
			

			//std::cout << "Get Image: ( " << image.rows << " | " << image.cols << " )" << std::endl;

		} // End of state 1

        usleep( 50000 );

	} // end of WHILE loop
}
		

void Sensor_Server::I2C_thread_funktion()
{
	logfile logger;
	logger.open("senser_connection_logfile");

	logger << "Start thread function";

	IMU_Measurement last_meas = {0.0};
	int retry_counter = 0;		

	while( this->continue_server )
	{

		std::chrono::system_clock::time_point act_time;
		act_time = std::chrono::system_clock::now();


		/****************************
		 *  I2C Register Map
		 *----------------------------
		 *  0x00 - 0x01 Batterie Voltage (3 Zellen)
		 *  0x02 - 0x03 Batterie Voltage (Spannungsteiler 6 Zellen)
		 *  0x04 - 0x05 Sonar Back Right
		 *  0x06 - 0x07 Sonar Front Right
		 *  0x08 - 0x09 Sonar Front
		 *  0x0A - 0x0B Sonar Front Left
		 *  0x0C - 0x0D Sonar Back Left
		 *  0x0E        SW Watchdog Status Register
		 *  0x0F        (empty)
		 *  0x10 - 0x11 Right Wheel Rotations
		 *  0x12 - 0x13 Right Wheel Sensor Resistance
		 *  0x14 - 0x15 Left Wheel Rotations
		 *  0x16 - 0x17 Left Wheel Sensor Resistance
		 ****************************/

		// Read every register in the microcontroller
		char buffer[24];
		int uc_read_ret = -1;
		retry_counter = 0;

		do
		{
			uc_read_ret = i2c_bus.i2c_read( 0x24, 0x00, 24, buffer );

			if( uc_read_ret < 0 )
			{
				std::string errormsg = "Error while reading µC: 0x24, 0x00 -> ";
				errormsg += std::string( std::to_string(uc_read_ret) );
				logger << errormsg;
			}

			
		} while( uc_read_ret < 0 && ++retry_counter < 3 );
		
		if( uc_read_ret < 0 )
		{
			std::cout << "Fehler beim lesen des uc: " << uc_read_ret 
				<< "; " << errno << ": " << strerror( errno) << std::endl;
		}		

		// only process, if reading was succesfull
		else
		{
			// Read ADC Values
			ADC_Measurement voltage;		 
	
			voltage.ADC_low = (uint16_t) (buffer[0] | ( buffer[1] << 8 ));
			voltage.ADC_low *= 0.007; // Konvert to voltage
			voltage.ADC_high = (uint16_t) (buffer[2] | ( buffer[3] << 8 ));
			voltage.ADC_high *= 0.007;

			voltage.timestamp = act_time;

			adc_queue_mutex.lock();
			this->adc_values.push_front(voltage);
			while( this->adc_values.size() > 5 )
			{
				this->adc_values.pop_back();
			}
			adc_queue_mutex.unlock();
		
		
			// Calculate the distances
			uint16_t sonar_value[5];
			Sonar_Measurement Sonar_meas;

			sonar_value[0] = (uint16_t) (buffer[4] | ( buffer[5] << 8 ));
			sonar_value[1] = (uint16_t) (buffer[6] | ( buffer[7] << 8 ));
			sonar_value[2] = (uint16_t) (buffer[8] | ( buffer[9] << 8 ));
			sonar_value[3] = (uint16_t) (buffer[10] | ( buffer[11] << 8 ));
			sonar_value[4] = (uint16_t) (buffer[12] | ( buffer[13] << 8 ));

			Sonar_meas.BackRight = 3. + 0.1715 * sonar_value[0];
			Sonar_meas.FrontRight = 3. + 0.1715 * sonar_value[1];	
			Sonar_meas.Front = 3. + 0.1715 * sonar_value[2];	
			Sonar_meas.FrontLeft = 3. + 0.1715 * sonar_value[3];	
			Sonar_meas.BackLeft = 3. + 0.1715 * sonar_value[4];	
			Sonar_meas.timestamp = act_time;

			Sonar_queue_mutex.lock();
			Sonar_values.push(Sonar_meas);
			while( Sonar_values.size() > 5 )
			{
				Sonar_values.pop();
			}
			Sonar_queue_mutex.unlock();	
		
			//std::cout << "Front: " << Sonar_meas.Front << std::endl;
		
		
			// Cals the wheel decoder messages
			Wheel_Measurement wheel_meas;
			// 0 - right, 1 - left
			uint16_t wheel_decoder_resistance[2];
			uint16_t wheel_decoder_counter[2];

			wheel_decoder_resistance[0] = (uint16_t) (buffer[18] | ( buffer[19] << 8 ));
			wheel_decoder_resistance[1] = (uint16_t) (buffer[22] | ( buffer[23] << 8 ));

			wheel_decoder_counter[0] = (uint16_t) (buffer[16] | ( buffer[17] << 8 ));
			wheel_decoder_counter[1] = (uint16_t) (buffer[20] | ( buffer[21] << 8 ));

		
			wheel_meas.Right_Wheel_Rotations = wheel_decoder_counter[0];
			wheel_meas.Left_Wheel_Rotations = wheel_decoder_counter[1];
			wheel_meas.timestamp = act_time;


		        Wheel_queue_mutex.lock();
	        	this->Wheel_values.push_front(wheel_meas );
		        while( this->Wheel_values.size() > 5 )
	        	{
	                	this->Wheel_values.pop_back();
			}
        		Wheel_queue_mutex.unlock();
		
		}

		//std::cout << "Read content of the imu" << std::endl;

		// Get the IMU Values
		IMU_Measurement IMU_meas;


		// Read Temperature
		int16_t temp_values;
		retry_counter = 0;
		int imu0_read_ret = -1;		

		do
		{	
			imu0_read_ret = i2c_bus.i2c_read( 0x6b, 0x15, 2, (char*) &temp_values );

			//std::cout << "Read: " << (int) IMU_buffer << std::endl;

			if( imu0_read_ret < 0 )
			{
				std::string errormsg = "Error while reading imu: 0x68, 0x3B -> ";
				errormsg += std::string( std::to_string(imu0_read_ret) );
				logger << errormsg;
			}

		} while( imu0_read_ret < 0 && ++retry_counter < 3 );

		if( imu0_read_ret < 0 )
		{
			std::cout << "Fehler beim lesen dere IMU: " << imu0_read_ret
				<< "; " << errno << ": " << strerror( errno) << std::endl;
		}
		else
		{	
			/*std::cout << (temp_values / 16.0) + 25.0  << std::endl;*/
			
			IMU_meas.temp = (temp_values / 16.0) + 25.0;
		}


		// Read Gyroscope
        gyroscope_val_t meas;
        if( this->gyroscope->readValues() >= 0 )
        {
            meas = this->gyroscope->getValues();
            IMU_meas.gyro[0] = meas.x_val;
            IMU_meas.gyro[1] = meas.y_val;
            IMU_meas.gyro[2] = meas.z_val;
        }


		// Read Accelerometer
		int16_t acc_buffer[3];
		retry_counter = 0;
		int imu2_read_ret = -1;		

		do
		{	
			imu2_read_ret = i2c_bus.i2c_read( 0x6b, 0x28, 6, (char*) acc_buffer );

			//std::cout << "Read: " << (int) IMU_buffer << std::endl;

			if( imu2_read_ret < 0 )
			{
				std::string errormsg = "Error while reading imu: 0x68, 0x3B -> ";
                errormsg += std::string( std::to_string(imu2_read_ret) );
				logger << errormsg;
			}



		} while( imu2_read_ret < 0 && ++retry_counter < 3 );

		if( imu2_read_ret < 0 )
		{
            std::cout << "Fehler beim lesen dere IMU: " << imu2_read_ret
				<< "; " << errno << ": " << strerror( errno) << std::endl;
		}
		else
		{
			/*std::cout 	<< "x: " << (acc_buffer[0] * 0.068)/1000.0 << "\n"
					<< "y: " << (acc_buffer[1] * 0.068)/1000.0 << "\n"
					<< "z: " << (acc_buffer[2] * 0.068)/1000.0 << std::endl;*/

			IMU_meas.acc[0] = (acc_buffer[0] * 0.068)/1000.0;
			IMU_meas.acc[1] = (acc_buffer[1] * 0.068)/1000.0;
			IMU_meas.acc[2] = (acc_buffer[2] * 0.068)/1000.0;

		}



	
		// Read Magnetometer
		if( this->magnetometer->readValues() >= 0 )
        {
            IMU_meas.mag = this->magnetometer->getValues();
        }


		IMU_meas.timestamp = act_time;

		IMU_queue_mutex.lock();
		this->IMU_values.push_front( IMU_meas );
		while( this->IMU_values.size() > 5 )
		{
			this->IMU_values.pop_back();
		}
		IMU_queue_mutex.unlock();
				
		usleep( 10000 );

	} // End of while loop

	logger << "End thread function";
	logger.close();

}

struct sockaddr_in Sensor_Server::getSocketAdressByFh( int fh )
{
	for( client_socket_entry iter : *clients )
	{
		if( iter.socket_fh ) return iter.name;
	}

	struct sockaddr_in ret;
	return ret;
}

struct timeval timeval_difference( struct timeval end, struct timeval begin )
{
	struct timeval ret;

	ret.tv_sec = end.tv_sec - begin.tv_sec;
	ret.tv_usec = end.tv_usec - begin.tv_usec;

	if( ret.tv_usec < 0 )
	{
		ret.tv_sec -= 1;
		ret.tv_usec += 1000000;
	}

	return ret;
}

// Returns the difference in milli Seconds
double time_difference( struct timeval end, struct timeval begin )
{
	struct timeval ret;

	ret.tv_sec = end.tv_sec - begin.tv_sec;
	ret.tv_usec = end.tv_usec - begin.tv_usec;

	if( ret.tv_usec < 0 )
	{
		ret.tv_sec -= 1;
		ret.tv_usec += 1000000;
	}

    return ((ret.tv_sec  * 1000.0) + (ret.tv_usec / 1000.0)) ;
}

bool operator==(struct timeval end, struct timeval begin )
{
	return ( end.tv_sec == begin.tv_sec  &&  end.tv_usec == begin.tv_usec );
}

/**
 * @brief evaluate_options - Interpret and set the options
 * @param argc - number of agruments
 * @param argv - array with the arguments
 */
void Sensor_Server::evaluate_options( int argc, char** argv )
{
	// Convert the input to a string vector
	std::vector<std::string> arguments;
	for( int i = 0; i < argc; i++ )
	{
		arguments.push_back(std::string( argv[i] ));
	}

	// Go through the options
	for( int i = 1; i < arguments.size(); i++ )
	{
		std::string begining = arguments.at(i).substr(0,2);
		std::string end = arguments.at(i).substr(2,arguments.at(i).size());


		if( begining == "-c" )
		{
			if( end == "magn" )
			{
				this->options.calibrate_magnetomer = true;
			}
            else if( end == "gyro")
            {
                this->options.calibrate_gyroscope = true;
            }
			else
			{
				std::cout << "Unknown Calibration: \"" << end << "\"" << std::endl;
			}			
		}

		else if( begining == "-h" )
		{		
			this->options.show_help = true;
		}

		else if( begining == "-d" )
		{
			std::cout << "Start in dummy mode" << std::endl;
			this->options.dummy_mode = true;
		}


		else
		{
			std::cout << "Unknown argument: " << arguments.at(i) << std::endl;
		}		
	}
}

/**
 * @brief printHelp - Prints help in the console
 */
void Sensor_Server::printHelp()
{
	std::cout << "Sensor Server" << "\n\n"
		<< "-h : Print this help message.\n\n"
        << "-cmagn : Calibrate the magnetometer.\n"
        << "-cgyro : Calibrate the gyroscope.\n"
		<< "-d : start in dummy mode. No real sensor data will be send."
	<< std::endl;
}

