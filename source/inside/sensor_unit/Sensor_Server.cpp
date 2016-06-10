#include "Sensor_Server.hpp"

#include <fstream>

logfile log_file;

Sensor_Server::Sensor_Server( int portnr ) : Server_inet( portnr )
{
	this->start_up =true;
}

Sensor_Server::~Sensor_Server()
{}

void Sensor_Server::setup()
{
    log_file.open( "logfile");

    i2c_bus.open_connection();

	this->cam_state = 0;
	
	CAM_thread = std::thread( &Sensor_Server::CAM_thread_funktion, this);
	I2C_thread = std::thread( &Sensor_Server::I2C_thread_funktion, this); 


	working_thread = std::thread( &Sensor_Server::working_thread_function, this );

	// enable IMU
	 i2c_bus.i2c_write<uint8_t>( 0x68, 0x6B, 0x00  ); //disable sleep mode

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
            case 1: left_direction = 0; break;
            case 2: left_direction = 1; break;
            case 4: left_direction = -1; break;
            default: break;
            }

            switch( new_value[1])
            {
		case 1: right_direction = 0; break;
		case 2: right_direction = 1; break;
            	case 4: right_direction = -1; break;
            	default: break;
            }

            break;

        default:
            break;
        }

    }

    else
    {
        // Nethertheless read the bytes
        uint8_t data[100];

        std::cout << "Unknown Command. Read " << headder[1] << " Bytes" << std::endl;

        int ret = read( client_handle , data, headder[1] );
    }



    if( headder[0] == 1 )
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

			gettimeofday( &image.timestamp,NULL);

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

	IMU_Measurement last_meas = {0.0};

	while( this->continue_server )
	{

		struct timeval act_time;
		gettimeofday( &act_time, NULL);


		//std::cout << "Thread function" << std::endl;
		
		//std::cout << "Read complete content of microcontroller" << std::endl;

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
		i2c_bus.i2c_read( 0x24, 0x00, 24, buffer );
		

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

		//std::cout << "Read content of the imu" << std::endl;


		// Get the IMU Values
		IMU_Measurement IMU_meas;


		// Magnetometer
		char IMU_buffer[8];
		i2c_bus.i2c_read( 0x24, 0x03, 6, IMU_buffer );
		int16_t mag_values[3];

		const float influence = 0.02;

		for( int i = 0; i<3; i++ )
		{	
			mag_values[i] = (uint16_t) (IMU_buffer[2*i+1] | ( IMU_buffer[2*i] << 8 ));	
			IMU_meas.mag[i] = (1- influence) * last_meas.mag[i] + influence * (mag_values[i] * 0.3001221) ;
		}


		// Accelerometer
		i2c_bus.i2c_read( 0x24, 0x3B, 6, IMU_buffer );
		uint16_t acc_values[3];
		
		for( int i = 0; i<3; i++ )
		{
			acc_values[i] = (uint16_t) (IMU_buffer[2*i+1] | ( IMU_buffer[2*i] << 8 ));
			IMU_meas.acc[i] = acc_values[i] / 16384.0 ;
		}


		// Gyroskope and Temperature
		i2c_bus.i2c_read( 0x24, 0x41, 6, IMU_buffer );
		
		int16_t temp_value;
		temp_value = (int16_t) (IMU_buffer[1] | ( IMU_buffer[0] << 8 ));
		IMU_meas.temp = (temp_value - 521) / 340;


		int16_t gyro_values[3];	
	
		for( int i = 1; i<4; i++ )
		{
			gyro_values[i] = (uint16_t) (IMU_buffer[2*i+1] | ( IMU_buffer[2*i] << 8 ));
			IMU_meas.gyro[i] = gyro_values[i] / 131.;
		}


		IMU_meas.timestamp = act_time;


		IMU_queue_mutex.lock();
			IMU_values.push_back( IMU_meas );
			while( IMU_values.size() > IMU_Buffer_Size )
			{
				IMU_values.pop_front();
			}	
		IMU_queue_mutex.unlock();

		last_meas = IMU_meas;


		//usleep( 500000 );
		usleep( 10000 );

	} // End of while loop
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
