#include <sys/time.h>
#include "ivymike/time.h"


double ivy_mike::gettime(void )
{
#ifdef WIN32
  time_t tp;
  struct tm localtm;
  tp = time(NULL);
  localtm = *localtime(&tp);
  return 60.0*localtm.tm_min + localtm.tm_sec;
#else
  struct timeval ttime;
  gettimeofday(&ttime , 0);
  return ttime.tv_sec + ttime.tv_usec * 0.000001;
#endif
}