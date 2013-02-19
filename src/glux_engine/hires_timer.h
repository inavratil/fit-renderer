#ifndef _HIRES_TIMER
#define _HIRES_TIMER

#if defined (__WIN32__) || defined(_WIN32)
	#define __WINDOWS
	#include <windows.h>
#else
	#define __LINUX
	#include <sys/time.h>
	#include <time.h>
#endif

/*
 * High Resolution Timer class
 * Supports Windows/Linux
 */

class HRTimer
{
private:	

	#ifdef __WINDOWS
	LARGE_INTEGER startCount, stopCount, countsPerSecond, timeElapsed;
	#endif

	#ifdef __LINUX
	ulong startCount, stopCount, timeElapsed;
	#endif

public:

	//constructor
	HRTimer()
	{
		#ifdef __WINDOWS		
		QueryPerformanceFrequency( &countsPerSecond );
		#endif
		Reset();
	}

	//reset timer
	void Reset()
	{ 
		#ifdef __WINDOWS
		QueryPerformanceCounter( &startCount );
		#endif
		
		#ifdef __LINUX
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		startCount = ts.tv_nsec;	//in nanoseconds
		#endif

	}
	void Start()
	{ 
		Reset(); 
	}

	//return elapsed times
	inline double GetElapsedTimeSeconds()
	{					
		#ifdef __WINDOWS
		QueryPerformanceCounter( &stopCount );	
		timeElapsed.QuadPart = stopCount.QuadPart - startCount.QuadPart;			
		return (double) timeElapsed.QuadPart / countsPerSecond.QuadPart ;		
		#endif

		#ifdef __LINUX
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		stopCount = ts.tv_nsec;
		timeElapsed = stopCount - startCount;

		return (double) timeElapsed/10e9;
		#endif
	}
	inline double GetElapsedTimeMilliseconds()
	{		
		#ifdef __WINDOWS
		QueryPerformanceCounter( &stopCount );	
		timeElapsed.QuadPart = stopCount.QuadPart - startCount.QuadPart;			
		return (double) timeElapsed.QuadPart / countsPerSecond.QuadPart * 1000 ;		
		#endif

		#ifdef __LINUX
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		stopCount = ts.tv_nsec;
		timeElapsed = stopCount - startCount;

		return (double) timeElapsed/10e6;
		#endif
	}
};	

#endif