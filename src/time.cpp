



#ifndef WIN32
#include <sys/time.h>
#else 
#include "ivymike/disable_shit.h"

#include <windows.h>
static bool g_pc_valid = false;
static LARGE_INTEGER g_pc_freq;
#endif
#include "ivymike/time.h"


double ivy_mike::gettime(void )
{
#ifdef WIN32
	if( !g_pc_valid ) {
		QueryPerformanceFrequency( &g_pc_freq );
		g_pc_valid = true;
	} 
	LARGE_INTEGER pcv;
	QueryPerformanceCounter( &pcv );
	return pcv.QuadPart / double(g_pc_freq.QuadPart);
	
#else
 struct timeval ttime;
 gettimeofday(&ttime , 0);
 return ttime.tv_sec + ttime.tv_usec * 0.000001;
#endif
}