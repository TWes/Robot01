#include "time_helper.hpp"

namespace util
{

time_t getActTime()
{
	time_t act_time;
	gettimeofday( &act_time, 0);

	return act_time;
}


time_t calcDuration_ms( float duration )
{
	time_t ret;


	double duration_us  = duration * 1000;
		
	ret.tv_sec = (int) duration_us / (1000*1000);
	ret.tv_usec = (int) duration_us - (ret.tv_sec * 1000*1000);
	
	return ret;}

} // End of namespace

util::time_t operator+( util::time_t &left, util::time_t &right)
{
	util::time_t sum;
	
	double us_sum = left.tv_usec + right.tv_usec;
	int overflow_seconds = us_sum / (1000*1000);
	
	sum.tv_usec = us_sum - (overflow_seconds *1000*1000); 
	sum.tv_sec = left.tv_sec + right.tv_sec + overflow_seconds;

	return sum;
}

std::ostream& operator<< (std::ostream &out, util::time_t &time)
{
	out << time.tv_sec << "." << time.tv_usec;

	return out;
}

bool operator< ( util::time_t &left, util::time_t &right)
{
	bool smalerThan = false;

	if( left.tv_sec < right.tv_sec )
		smalerThan = true;

	else if( left.tv_sec == right.tv_sec &&
		left.tv_usec < right.tv_usec )
		smalerThan = true;

	return smalerThan;
}
