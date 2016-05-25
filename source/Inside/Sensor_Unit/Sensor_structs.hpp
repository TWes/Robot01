#ifndef SENSOR_STRUCT_HPP
#define SENSOR_STRUCT_HPP

#include <opencv2/imgproc/imgproc.hpp>

typedef struct
{
	cv::Mat image;
	
	struct timeval timestamp;

} CAM_image;

typedef struct
{
	float ADC_low;
	float ADC_high;

	struct timeval timestamp;

} ADC_Measurement;

typedef struct
{
	float gyro[3];
	float acc[3];
	float mag[3];
	float temp;

	struct timeval timestamp;

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
