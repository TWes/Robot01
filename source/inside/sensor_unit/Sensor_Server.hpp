#ifndef SENSOR_SERVER_HPP
#define SENSOR_SERVER_HPP

#include <mutex>
#include <queue>
#include <list>
#include <fstream>
#include <chrono>
#include <unistd.h>
#include <sys/time.h>
#include <iostream>
#include <algorithm>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "server_inet.hpp"
#include "i2c_access.hpp"
#include "Sensor_structs.hpp"
#include "logfile.hpp"
#include "sensor_protocol.hpp"
#include "udp_connection_inet.hpp"

#include "XMLWriter.hpp"


#include "sensor_interfaces/magnetometer_interface.hpp"
#include "sensor_interfaces/magnetometer_lsm9ds1.hpp"

/** Struct that contains every information needed
    to handle an udp subscription */
typedef struct{

    float next_sending_time;                    /// The next time a message has to be send
    float sending_interval;                     /// The intervall between to sendings
    uint32_t sending_object;                    /// what should be send
    udp::connection_information_t client_info;   /// client connection info
    int seq_number;                             /// id of subscription

} UDP_subscriber_entry_t;


/** Struct that contains the options passed
	when starting the server */
typedef struct{
	bool calibrate_magnetomer = false;	/// Shoult the Magnetometer be calibrated at the beginning?
	bool show_help = false;		/// just show help and exit
	bool dummy_mode = false	;	/// If this is true, the sensor server will send computer generated values
} option_struct_t;



/**
 * @brief The Sensor_Server class
 * The sensor server which reads and evaluates the sensors
 */
class Sensor_Server : public Server_inet
{
	public:
		Sensor_Server( int argc, char** argv, int portnr );
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

        magnetometer_interface *magnetometer;

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
        void dummy_thread_function();

		Position_t act_position;


		void setup();
		void handle_connection( int client_handle );
		void cleanup();

private:
	void evaluate_options( int argc, char** argv );
	option_struct_t options; // The choosen options
	void printHelp();

	struct sockaddr_in getSocketAdressByFh( int fh );

    udp::Socket udp_connection;
	std::vector<UDP_subscriber_entry_t> UDP_subscriber;
    void deleteIdInUDPSubscriber(int id);
	std::thread *udp_sending_thread;
	void udp_sending_function();

    Status_tuple_t act_status_tuple;
};


float calcMagnetometerOrientation( magnetometer_val_t magn_meas );


#endif
