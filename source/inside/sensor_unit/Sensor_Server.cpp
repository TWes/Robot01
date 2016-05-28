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

	ADC_thread = std::thread( &Sensor_Server::ADC_thread_funktion , this);
	Sonar_thread = std::thread( &Sensor_Server::Sonar_thread_funktion , this);
	IMU_thread = std::thread( &Sensor_Server::IMU_thread_funktion, this );
	CAM_thread = std::thread( &Sensor_Server::CAM_thread_funktion , this);
    Wheel_thread = std::thread( &Sensor_Server::Wheel_thread_funktion, this );

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

	ADC_thread.join();
	Sonar_thread.join();
	CAM_thread.join();
    IMU_thread.join();
    Wheel_thread.join();

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

void Sensor_Server::ADC_thread_funktion()
{
	while( this->continue_server )
	{
		//std::cout << "Get ADV values" << std::endl;

		uint16_t adc_value[2];
		ADC_Measurement voltage;		

		i2c_mutex.lock();
			adc_value[0] = i2c_bus.i2c_read<uint16_t>(0x24, 0x00);
			adc_value[1] = i2c_bus.i2c_read<uint16_t>(0x24, 0x02);
		i2c_mutex.unlock();

		voltage.ADC_low = adc_value[0] / 204.6;
		voltage.ADC_high = adc_value[1] / 204.6;

		gettimeofday( &voltage.timestamp,NULL);

		adc_queue_mutex.lock();
			this->adc_values.push(voltage);
			while( this->adc_values.size() > 5 )
			{
				this->adc_values.pop();
			}
		adc_queue_mutex.unlock();

		sleep( 10 );
	}
}

void Sensor_Server::Sonar_thread_funktion()
{
	while( this->continue_server )
	{
  		uint16_t sonar_value[5];
		float sonar_distances[5];
		Sonar_Measurement Sonar_meas;

		i2c_mutex.lock();
			sonar_value[0] = i2c_bus.i2c_read<uint16_t>(0x24, 0x04);
			sonar_value[1] = i2c_bus.i2c_read<uint16_t>(0x24, 0x06);
			sonar_value[2] = i2c_bus.i2c_read<uint16_t>(0x24, 0x08);
			sonar_value[3] = i2c_bus.i2c_read<uint16_t>(0x24, 0x0A);
			sonar_value[4] = i2c_bus.i2c_read<uint16_t>(0x24, 0x0C);
		i2c_mutex.unlock();

		 Sonar_meas.BackRight = 3. + 0.1715 * sonar_value[0];
		 Sonar_meas.FrontRight = 3. + 0.1715 * sonar_value[1];	
		 Sonar_meas.Front = 3. + 0.1715 * sonar_value[2];	
		 Sonar_meas.FrontLeft = 3. + 0.1715 * sonar_value[3];	
		 Sonar_meas.BackLeft = 3. + 0.1715 * sonar_value[4];	
	
		gettimeofday( &Sonar_meas.timestamp,NULL);

		Sonar_queue_mutex.lock();
			Sonar_values.push(Sonar_meas);
			while( Sonar_values.size() > 5 )
			{
				Sonar_values.pop();
			}
		Sonar_queue_mutex.unlock();	

        usleep( 100000  );
	}
}

void Sensor_Server::IMU_thread_funktion()
{
	IMU_Measurement last_meas = {0.0};

	while( this->continue_server )
	{
		int16_t gyro_values[3];
		 IMU_Measurement meas;

		i2c_mutex.lock();
			gyro_values[0] = i2c_bus.i2c_read<int16_t>(0x68, 0x43);
			gyro_values[1] = i2c_bus.i2c_read<int16_t>(0x68, 0x45);
			gyro_values[2] = i2c_bus.i2c_read<int16_t>(0x68, 0x47);
		i2c_mutex.unlock();
		
		for( int i = 0; i<3; i++ )
		{
			int16_t buf = 0xff & ( gyro_values[i] >> 8 );
			buf =  0xff00 & ( gyro_values[i] << 8 );

			meas.gyro[i] = buf / 131.;

			//std::cout << "Axis " << i << ": " << meas.gyro[i] << std::endl;	
		}

		// Accelerometer meas
		uint16_t acc_values[3];

		i2c_mutex.lock();
			acc_values[0] = i2c_bus.i2c_read<uint16_t>(0x68, 0x3B);
			acc_values[1] = i2c_bus.i2c_read<uint16_t>(0x68, 0x3D);
			acc_values[2] = i2c_bus.i2c_read<uint16_t>(0x68, 0x3F);
		i2c_mutex.unlock();

		for( int i = 0; i<3; i++ )
		{
			int16_t buf = 0xff & ( acc_values[i] >> 8 );
			buf +=  0xff00 & ( acc_values[i] << 8 );

			meas.acc[i] = buf / 16384.0 ;

			//std::cout << "Axis " << i << ": " << meas.acc[i] << std::endl;	
		}
		
		// Read temperature value
		int16_t temp_value;
		temp_value = i2c_bus.i2c_read<int16_t>(0x68, 0x41);
	
		{
			int16_t buf = 0xff & ( temp_value >> 8 );
			buf =  0xff00 & ( temp_value << 8 );
			meas.temp = (buf - 521) / 340;

			//std::cout << "Temp: " << meas.temp << std::endl;	
		}

		// read magnetometer values
		int16_t mag_values[3];

		i2c_mutex.lock();
			mag_values[0] = i2c_bus.i2c_read<int16_t>(0x68, 0x03);
			mag_values[1] = i2c_bus.i2c_read<int16_t>(0x68, 0x05);
			mag_values[2] = i2c_bus.i2c_read<int16_t>(0x68, 0x07);
		i2c_mutex.unlock();

		const float influence = 0.02;

		for( int i = 0; i<3; i++ )
		{			
			meas.mag[i] = (1- influence) * last_meas.mag[i] + influence * (mag_values[i] * 0.3001221) ;
		}

		gettimeofday( &meas.timestamp,NULL);

		IMU_queue_mutex.lock();
			IMU_values.push_back(meas );
			while( IMU_values.size() > IMU_Buffer_Size )
			{
				IMU_values.pop_front();
			}	
		IMU_queue_mutex.unlock();

		last_meas = meas;

		usleep( 50000 );
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

void Sensor_Server::Wheel_thread_funktion()
{
    while( this->continue_server )
    {
        Wheel_Measurement wheel_meas;

        i2c_mutex.lock();
            wheel_meas.Right_Wheel_Rotations = i2c_bus.i2c_read<uint16_t>(0x24, 0x10);
            wheel_meas.Left_Wheel_Rotations = i2c_bus.i2c_read<uint16_t>(0x24, 0x14);
        i2c_mutex.unlock();

        gettimeofday( &wheel_meas.timestamp, NULL);

	//std::cout << "got: " << wheel_meas.Right_Wheel_Rotations << std::endl;

         Wheel_queue_mutex.lock();
            this->Wheel_values.push_front(wheel_meas );
            while( this->Wheel_values.size() > 5 )
            {
                this->Wheel_values.pop_back();
            }
        Wheel_queue_mutex.unlock();

        usleep( 100000 );
    }
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
