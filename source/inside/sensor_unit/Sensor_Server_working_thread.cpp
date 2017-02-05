#include "Sensor_Server.hpp"


/**
 * @brief Sensor_Server::working_thread_function
 * A thread function which calculates all the sensor values
 * together.
 */
void Sensor_Server::working_thread_function()
{
    act_status_tuple.linear_velocity[0] = 0.0;
    act_status_tuple.linear_velocity[1] = 0.0;
    act_status_tuple.linear_velocity[2] = 0.0;

    act_status_tuple.angular_velocity[0] = 0.0;
    act_status_tuple.angular_velocity[1] = 0.0;
    act_status_tuple.angular_velocity[2] = 0.0;

    while( this->continue_server )
    {
	const float wheel_distance = 0.1; // 10 cm
	const float delay_time = 10000; // 0.01 s

        static Wheel_Measurement last_wheel_meas;

	static IMU_Measurement last_imu_meas;

	/***************************
	* Get all the measurements
	***************************/
	ADC_Measurement act_adc_measurment;		
	adc_queue_mutex.lock();
		act_adc_measurment = *(adc_values.begin());
	adc_queue_mutex.unlock();

	IMU_Measurement act_imu_meas;
	IMU_queue_mutex.lock();
            act_imu_meas = *(IMU_values.begin());
	IMU_queue_mutex.unlock();

	Wheel_Measurement act_wheel_meas;
	Wheel_queue_mutex.lock();
            act_wheel_meas = *(Wheel_values.begin());
	Wheel_queue_mutex.unlock();


	/***********************************
	* Calgulate the batterie voltage
	************************************/
	/*std::cout << "ADC L: " << act_adc_measurment.ADC_low << std::endl;
	std::cout << "ADC H: " << act_adc_measurment.ADC_high << std::endl;*/
	act_status_tuple.BatteryLow = act_adc_measurment.ADC_low;
	act_status_tuple.BatteryHigh = act_adc_measurment.ADC_high;


	/***********************************
	* Calgulate the orientation change 
	* out of the magnetometer
	************************************/
	static float last_magn_orientation = -1.0;
	std::chrono::milliseconds tmp_time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(act_imu_meas.timestamp - last_imu_meas.timestamp);
	double magn_delta_t = tmp_time_diff.count() / 1000.0;

	double magnAngVelZ = 0.0;	

	if( magn_delta_t <= 0.0 || magn_delta_t > 1000 )
	{}
	else
	{
		float act_magn_orientation = calcMagnetometerOrientation( act_imu_meas.mag );
		float magn_orientation_change = 0.0;

		if( last_magn_orientation >= 0.0 )
		{
			float abs_magn_change = act_magn_orientation - last_magn_orientation;
			
			magn_orientation_change = abs_magn_change / magn_delta_t;
		}

		last_magn_orientation = act_magn_orientation;

		magnAngVelZ = 0.7 * act_status_tuple.magnAngVelZ + 0.3 * magn_orientation_change;

		act_status_tuple.magnAngVelZ = magnAngVelZ;
	}
	

    /*****************************
    * Calculate the new Position out of
    * the steering commands
    ****************************/
	const float predicted_velocity = 0.7;
	
	float predicted_left_velocity = this->left_direction * predicted_velocity;
	float predicted_right_velocity = this->right_direction * predicted_velocity;	
	
	float predictedAngVelZ = (predicted_left_velocity - predicted_right_velocity)/ wheel_distance;
        float predictedForewardVel = 0.5 * (predicted_left_velocity + predicted_right_velocity);

   	act_status_tuple.predicdedAngVelZ = 0.5*  predictedAngVelZ + 0.5 * act_status_tuple.predicdedAngVelZ;
	act_status_tuple.predictedLineVelX = 0.5 * predictedForewardVel + 0.5 * act_status_tuple.predictedLineVelX;
	act_status_tuple.predictedLineVelY = 0;

	//std::cout << "Theta: " << predicted_orientation_change << std::endl;
	
	
	/*****************************
         * Calculate the new Position out of
         * the decoder
         ****************************/
	// Get Time Difference and convert it to seconds
	std::chrono::milliseconds tmp_wheel_time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(act_wheel_meas.timestamp - last_wheel_meas.timestamp);
	double delta_t = tmp_wheel_time_diff.count() / 1000.0; // Delta t in seconds

	static double decoder_left_velocity = 0.0;
	static double decoder_right_velocity = 0.0;
	static double decoder_ang_velocity = 0.0;
	static double decoder_fwd_velocity = 0.0;

	// No new measurements
        if( delta_t == 0.0 )
        {
	   //std::cout << " Wheel delta_t = " << delta_t << std::endl;
        }
	else
	{
        // Process the wheel encoder
        // 1. Calculate the distance driven
        int right_steps_since_last = act_wheel_meas.Right_Wheel_Rotations - last_wheel_meas.Right_Wheel_Rotations;
        int left_steps_since_last = act_wheel_meas.Left_Wheel_Rotations - last_wheel_meas.Left_Wheel_Rotations;

        // in Meter
        const double wheel_circumference = 0.1225;
	const double decoder_steps_to_m = 0.01071875;

        // the direction variable depends on the direction send by the GPIO Server
        double right_distance_since_last = (right_direction ? right_direction : 1) * \
        	right_steps_since_last * decoder_steps_to_m;
        double left_distance_since_last = (left_direction ? left_direction : 1) * \
		left_steps_since_last * decoder_steps_to_m;
	

        // -1 back; 0 - No movement; 1 foreward
        double right_velocity = (right_distance_since_last) / delta_t;
        double left_velocity = (left_distance_since_last) / delta_t;

	// Throuought test we found out, that the robot
        // couldnt move faster than that
        const double max_velocity = 1.0;

        // Has to be some Kind of error
        // last_measurement is not updated
        if( right_velocity > max_velocity ||  right_velocity < -max_velocity ||
            left_velocity > max_velocity  || left_velocity < -max_velocity )
        {
		//std::cout << "velocity out of bound" << std::endl;
		//std::cout << "count diff: " << left_steps_since_last 
			//<< "; " << left_steps_since_last << std::endl;
	}
	else
	{
		// Succesfully calcuated
		decoder_left_velocity = 0.7 * decoder_left_velocity + 0.3 * left_velocity;
		decoder_right_velocity = 0.7 * decoder_right_velocity + 0.3 * right_velocity;

		decoder_ang_velocity = (decoder_right_velocity - decoder_left_velocity) / wheel_distance;
		decoder_fwd_velocity = (decoder_right_velocity + decoder_left_velocity) / 2.0;	
	}	
	}
	

	/*****************************
         * Calculate the new Position out of
         * the IMU
         ****************************/	
	// Get Time Difference and convert it to seconds
	std::chrono::milliseconds tmp_imu_time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(act_imu_meas.timestamp - last_imu_meas.timestamp);
	double imu_delta_t = tmp_imu_time_diff.count() / 1000.0; // delta t in seconds

	
	float gyroAngVelX, gyroAngVelY, gyroAngVelZ;
	float accLineVelX, accLineVelY, accLineVelZ;

	if( delta_t == 0.0 || delta_t > 1000.0 )
	{
		//std::cout << " IMU delta_t = " << imu_delta_t << std::endl;		
	}
	else
	{		
		gyroAngVelX = act_imu_meas.gyro[0];
		gyroAngVelY = act_imu_meas.gyro[1];
		gyroAngVelZ = act_imu_meas.gyro[2];
		

		accLineVelX = (act_imu_meas.acc[0]) * delta_t;
		accLineVelY = (act_imu_meas.acc[1]) * delta_t;
		accLineVelZ = (act_imu_meas.acc[2]) * delta_t;
	}

	act_status_tuple.gyroAngVel[0] = gyroAngVelX;
	act_status_tuple.gyroAngVel[1] = gyroAngVelY;
	act_status_tuple.gyroAngVel[2] = gyroAngVelZ;

	act_status_tuple.accLineVel[0] = accLineVelX;
	act_status_tuple.accLineVel[1] = accLineVelY;
	act_status_tuple.accLineVel[2] = accLineVelZ;

	
	/************************
	* Combine all the values
	*************************/
	act_status_tuple.angular_velocity[0] = gyroAngVelX;
	act_status_tuple.angular_velocity[1] = gyroAngVelY;
	act_status_tuple.angular_velocity[2] = gyroAngVelZ * 0.5 + magnAngVelZ * 0.5 ;
		

	// Calculate the velocity out of accelerometer
	act_status_tuple.linear_velocity[0] = 0.5 * decoder_fwd_velocity + 0.5 * accLineVelX;
	act_status_tuple.linear_velocity[1] = 0;
	act_status_tuple.linear_velocity[2] = 0;

    	last_wheel_meas = act_wheel_meas;
	last_imu_meas = act_imu_meas;

        usleep(delay_time);
    }
}

/**
 * @brief Sensor_Server::dummy_thread_function
 * A dummy thread function which sets the status values
 * to predefined values.
 */
void Sensor_Server::dummy_thread_function()
{
    const float delay_time = 100000; // 0.1 s

    while( this->continue_server )
    {
        // fill IMU_Measurement last_imu_meas;
        IMU_Measurement act_measurment_simulation;
        act_measurment_simulation.acc[0] = 0.0;
        act_measurment_simulation.acc[0] = 0.0;
        act_measurment_simulation.acc[0] = 1.0;

        act_measurment_simulation.gyro[0] = 0.0;
        act_measurment_simulation.gyro[1] = 0.0;
        act_measurment_simulation.gyro[2] = 0.0;

        act_measurment_simulation.mag.x_val = 1.0;
        act_measurment_simulation.mag.y_val = 0.0;
        act_measurment_simulation.mag.z_val = 0.0;

        act_measurment_simulation.temp = 23.7;
        act_measurment_simulation.timestamp = std::chrono::system_clock::now();

        IMU_queue_mutex.lock();
        this->IMU_values.push_front( act_measurment_simulation );
        while( this->IMU_values.size() > 5 )
        {
            this->IMU_values.pop_back();
        }
        IMU_queue_mutex.unlock();

        // set
        //act_status_tuple
        Status_tuple_t status_simulation;
        status_simulation.accLineVel[0] = 0.0;
        status_simulation.accLineVel[1] = 0.0;
        status_simulation.accLineVel[2] = 0.0;

   

        this->act_status_tuple = status_simulation;

        usleep(delay_time);
    }
}
