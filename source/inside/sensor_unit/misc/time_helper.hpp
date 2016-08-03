#ifndef TIME_HELPER_HPP
#define TIME_HELPER_HPP

#include <sys/time.h>
#include <iostream>

/*
 tv_sec      seconds
 tv_usec     microseconds
*/

namespace util
{
	typedef struct timeval time_t;

	time_t getActTime();

	time_t calcDuration_ms( float duration );
}

util::time_t operator+( util::time_t &left, util::time_t &right);

std::ostream& operator<< (std::ostream &out, util::time_t &time);

bool operator< ( util::time_t &left, util::time_t &right);

#endif
