/**
****************************************************************************************************
****************************************************************************************************
@file: timer.h
@brief timer class which measures time with precision in microseconds
****************************************************************************************************
***************************************************************************************************/
#ifndef _TIMER_H_
#define _TIMER_H_

#include "globals.h"

/**
@class Timer
@brief contains variables to start, stop and determine frequency on timer (Windows)
***************************************************************************************************/
class Timer
{
	private:
			__int64 start, stop, freq;

	public:
			///@brief initialize timer frequency (Windows)
			Timer()
			{ QueryPerformanceFrequency((LARGE_INTEGER *)&freq); }
			///@brief Start timer by setting start time value
			void Start()
			{ QueryPerformanceCounter((LARGE_INTEGER *)&start); }
			///@brief Stop timer and return elapsed time (in miliseconds)
			double Stop()
			{
				QueryPerformanceCounter((LARGE_INTEGER *)&stop); 
				return 1000.0*double(stop - start)/freq;
			}
};
#endif
