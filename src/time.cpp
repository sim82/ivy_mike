



#ifndef WIN32
#include <sys/time.h>
#else 
#include <ctime>
#endif
#include "ivymike/time.h"


double ivy_mike::gettime(void )
{
#ifdef WIN32
 return clock() / double(CLOCKS_PER_SEC);
#else
 struct timeval ttime;
 gettimeofday(&ttime , 0);
 return ttime.tv_sec + ttime.tv_usec * 0.000001;
#endif
}