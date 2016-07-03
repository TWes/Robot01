#ifndef SENSOR_SERVER_HPP
#define SENSOR_SERVER_HPP

#include <mutex>
#include <queue>
#include <list>
#include <fstream>

#include <unistd.h>
#include <sys/time.h>
#include <iostream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "server_inet.hpp"
#include "i2c_access.hpp"
#include "Sensor_structs.hpp"
#include "logfile.hpp"
#include "sensor_protocol.hpp"
#include "udp_connection_inet.hpp"

typedef struct{

	float next_sending_time;
	float sending_interval;
	uint32_t sending_object;
	udp_connection_information_t client_info;
	int seq_number;

} UDP_subscriber_entry_t;


class Sensor_Server : public Server_inet
{
	public:
		Sensor_Server( int portnr );
		~Sensor_Server();

		std::mutex i2c_mutex;
		i2c_access i2c_bus;

		bool start_up;

      		/* Variables need for calcualtation */
	        signed char left_direction;
        	signed char right_direction;

		// Members for the i2c communication
		void I2C_thread_funktion();
		std::thread I2C_thread;


		// Members for ADC		
		std::mutex adc_queue_mutex;
		std::list<ADC_Measurement> adc_values;

		// Members for Sonar
		std::mutex Sonar_queue_mutex;
		std::queue<Sonar_Measurement> Sonar_values;

        	// Members for Wheelencoder
	        std::mutex Wheel_queue_mutex;
	        std::list<Wheel_Measurement> Wheel_values;

		// Members for IMU
		std::mutex IMU_queue_mutex;
		std::list<IMU_Measurement> IMU_values;	
		static const int IMU_Buffer_Size = 1;	

		// Members for CAM
		void CAM_thread_funktion();
		std::thread CAM_thread;		
		cv::VideoCapture  webcam;
		int cam_state;
		std::mutex cam_queue_mutex;
		std::queue<CAM_image> cam_images;

		// Working thread
		std::thread working_thread;
		void working_thread_function();

		Position_t act_position;


		void setup();
		void handle_connection( int client_handle );
		void cleanup();
private:
	struct sockaddr_in getSocketAdressByFh( int fh );

	udp_connection_inet udp_connection;
	std::vector<UDP_subscriber_entry_t> UDP_subscriber;
	std::thread *udp_sending_thread;
	void udp_sending_function();
	

};


float calcMagnetometerOrientation( float x_val, float y_val, float z_val );


#endif
