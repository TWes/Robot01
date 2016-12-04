#ifndef SENSOR_STRUCT_HPP
#define SENSOR_STRUCT_HPP

#include <chrono>
#include <opencv2/imgproc/imgproc.hpp>

/** One measurement from the CAM  */
typedef struct
{
    cv::Mat image; /// The image taken in this measurement
	
    std::chrono::system_clock::time_point timestamp; /// Timestamp

} CAM_image;


/** One measurement from the voltage decoder */
typedef struct
{
    float ADC_low;  /// The measurement of half the batteries
    float ADC_high; /// The measurement of the whole batterie pack

    std::chrono::system_clock::time_point timestamp; /// Timestamp

} ADC_Measurement;


/** Struct containing the one magnetometer meas */
typedef struct
{
    float x_val; /// x-axis measurement
    float y_val; /// z-axis measurement
    float z_val; /// z-axis measurement

} magnetometer_val_t;


/** One IMU Measurement */
typedef struct
{
	float gyro[3];
	float acc[3];
    magnetometer_val_t mag; /// The magnetometer masurement
	float temp;

    std::chrono::system_clock::time_point timestamp; /// timestamp

} IMU_Measurement;

typedef struct
{
	float BackRight;
	float FrontRight;
	float Front;
	float FrontLeft;
	float BackLeft;

	std::chrono::system_clock::time_point timestamp;

} Sonar_Measurement;


typedef struct
{
    float BatteryLow;
    float BatteryHigh;	

    float magnAngVelZ;

    float predicdedAngVelZ;
    float predictedLineVelX;
    float predictedLineVelY;

    float gyroAngVel[3];
    float accLineVel[3];

    float angular_velocity[3];
    float linear_velocity[3];

} Status_tuple_t;


typedef struct
{
    int Right_Wheel_Rotations;
    int Left_Wheel_Rotations;

    std::chrono::system_clock::time_point timestamp;

} Wheel_Measurement;


typedef struct
{
	double x;
	double y;
	double theta;

} Position_t;


bool operator==(struct timeval end, struct timeval begin );
struct timeval timeval_difference( struct timeval end, struct timeval begin );
double time_difference( struct timeval end, struct timeval begin );

#endif
