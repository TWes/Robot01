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

#include "../connection_library/connection_library.hpp"
#include "i2c_access.hpp"
#include "Sensor_structs.hpp"
#include "logfile.hpp"


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

		// Members for ADC
		void ADC_thread_funktion();
		std::thread ADC_thread;
		std::mutex adc_queue_mutex;
		std::queue<ADC_Measurement> adc_values;

		// Members for Sonar
		void Sonar_thread_funktion();
		std::thread Sonar_thread;
		std::mutex Sonar_queue_mutex;
		std::queue<Sonar_Measurement> Sonar_values;

        // Members for Wheelencoder
        void Wheel_thread_funktion();
        std::thread Wheel_thread;
        std::mutex Wheel_queue_mutex;
        std::list<Wheel_Measurement> Wheel_values;

		// Members for IMU
		void IMU_thread_funktion();
		std::thread IMU_thread;
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

		void setup();
		void handle_connection( int client_handle );
		void cleanup();
};

#endif
