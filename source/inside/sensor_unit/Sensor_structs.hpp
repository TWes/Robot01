#ifndef SENSOR_STRUCT_HPP
#define SENSOR_STRUCT_HPP

#include <opencv2/imgproc/imgproc.hpp>

/** One measurement from the CAM  */
typedef struct
{
    cv::Mat image; /// The image taken in this measurement
	
    struct timeval timestamp; /// Timestamp

} CAM_image;


/** One measurement from the voltage decoder */
typedef struct
{
    float ADC_low;  /// The measurement of half the batteries
    float ADC_high; /// The measurement of the whole batterie pack

    struct timeval timestamp; /// Timestamp

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

    struct timeval timestamp; /// timestamp

} IMU_Measurement;

typedef struct
{
	float BackRight;
	float FrontRight;
	float Front;
	float FrontLeft;
	float BackLeft;

	struct timeval timestamp;

} Sonar_Measurement;


typedef struct
{
    int Right_Wheel_Rotations;
    int Left_Wheel_Rotations;

    struct timeval timestamp;

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
