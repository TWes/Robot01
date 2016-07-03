#include "Sensor_Server.hpp"

void Sensor_Server::working_thread_function()
{
    while( this->continue_server )
    {
	// Variables which contain the measurements
	// of the wheel encoder
        Wheel_Measurement act_wheel_meas;
        static Wheel_Measurement last_wheel_meas;

	static IMU_Measurement last_imu_meas;

	// The ADC Measurement
	ADC_Measurement act_adc_measurment;
		
	adc_queue_mutex.lock();
		act_adc_measurment = *(adc_values.begin());
	adc_queue_mutex.unlock();

	/*std::cout << "ADC L: " << act_adc_measurment.ADC_low << std::endl;
	std::cout << "ADC H: " << act_adc_measurment.ADC_high << std::endl;*/

	/***********************************
	* Calgulate the orientation change 
	* out of the magnetometer
	************************************/
     IMU_Measurement act_imu_meas;

     IMU_queue_mutex.lock();
            act_imu_meas = *(IMU_values.begin());
    IMU_queue_mutex.unlock();

    float act_magn_orientation = calcMagnetometerOrientation( act_imu_meas.mag );

	
    /*std::cout << "Magn Orientation: " << act_magn_orientation << std::endl; */



	/*****************************
         * Calculate the new Position out of
         * the decoder
         ****************************/	
	Wheel_queue_mutex.lock();
            act_wheel_meas = *(Wheel_values.begin());
       Wheel_queue_mutex.unlock();

	// Get Time Difference and convert it to seconds
        double delta_t = time_difference( act_wheel_meas.timestamp, last_wheel_meas.timestamp) / 1000.0;

	// No new measurements
        if( delta_t == 0.0 )
        {
	   std::cout << " Wheel delta_t = " << delta_t << std::endl;
        }
	else
	{
        // Process the wheel encoder
        // 1. Calculate the distance driven
        int right_steps_since_last = act_wheel_meas.Right_Wheel_Rotations - last_wheel_meas.Right_Wheel_Rotations;
        int left_steps_since_last = act_wheel_meas.Left_Wheel_Rotations - last_wheel_meas.Left_Wheel_Rotations;

        // in Meter
        const double wheel_circumference = 0.124;
	const double decoder_steps_to_m = 0.007920792;

        // Throuought test we found out, that the robot
        // couldnt move faster than that
        const double max_velocity = 1.0;

        double right_distance_since_last = (right_direction ? right_direction : 1) * \
                    right_steps_since_last * decoder_steps_to_m;
        double left_distance_since_last = (left_direction ? left_direction : 1) * \
                    left_steps_since_last * decoder_steps_to_m;

        // the direction variable depends on the direction send by the GPIO Server
        // -1 back; 0 - No movement; 1 foreward
        double right_velocity = (right_distance_since_last) / delta_t;
        double left_velocity = (left_distance_since_last) / delta_t;

        // Has to be some Kind of error
        // last_measurement is not updated
        if( right_velocity > max_velocity ||  right_velocity < -max_velocity ||
            left_velocity > max_velocity  || left_velocity < -max_velocity )
        {	

		//std::cout << "velocity out of bound" << std::endl;
		//std::cout << "count diff: " << left_steps_since_last 
			//<< "; " << left_steps_since_last << std::endl;

	}

	
	/*std::cout << "Decoder: " << std::endl;

	std::cout << "Steps: " << left_steps_since_last
		<< "; " << right_steps_since_last << std::endl;

	std::cout << "Distance: " << left_distance_since_last 
		<< "; " << right_distance_since_last << std::endl;		

	std::cout << "Velocity; " << left_velocity << "; " <<  right_velocity << std::endl;*/
	}

	

	/*****************************
         * Calculate the new Position out of
         * the decoder
         ****************************/	
	// Get Time Difference and convert it to seconds
        double imu_delta_t = time_difference( act_imu_meas.timestamp, last_imu_meas.timestamp) / 1000.0;

	if( delta_t == 0.0 )
	{
		std::cout << " IMU delta_t = " <<imu_delta_t << std::endl;		
	}
	else
	{

        double acceleration_IMU = act_imu_meas.acc[1]; // Beschleunigung in y- Richtung
        
	double rotation_IMU = act_imu_meas.gyro[2]; // rotation um die z- Achse

	float velocity[2];
	velocity[0] = act_imu_meas.acc[0] * imu_delta_t;
	velocity[1] = act_imu_meas.acc[1] * imu_delta_t;

	static double IMU_velocity;

	IMU_velocity = velocity[0] * 0.4 + 0.6 * IMU_velocity;
	
	if( IMU_velocity < 0.01 ) 
	{
		IMU_velocity = 0.0;
	}


	/*std::cout << "IMU_velocity" << IMU_velocity << std::endl;


	std::cout << "x: " << act_imu_meas.acc[0] << std::endl;
	std::cout << "y: " << act_imu_meas.acc[1] << std::endl;
	std::cout << "z: " << act_imu_meas.acc[2] << std::endl; */


	/*std::cout << "x: " << act_imu_meas.gyro[0] << std::endl;
	std::cout << "y: " << act_imu_meas.gyro[1] << std::endl;
	std::cout << "z: " << act_imu_meas.gyro[2] << std::endl; */


	static float x_pos = 0.0;

	x_pos += IMU_velocity * imu_delta_t;

	//std::cout << "X Pos: " << x_pos << std::endl;

	

	/*std::cout << "IMU Velo: " << IMU_velocity << std::endl;
	std::cout << "IMU Rotation: " << rotation_IMU << std::endl; */

	}
	

        last_wheel_meas = act_wheel_meas;
	last_imu_meas = act_imu_meas;

        usleep(500000);
    }
}

