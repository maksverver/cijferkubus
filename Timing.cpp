#include "Timing.h"

#include <sys/time.h>
#include <stdlib.h>

static struct timeval tv_now()
{
    struct timeval tv;
    (void)gettimeofday(&tv, NULL);
    return tv;
}

static struct timeval tv_start = tv_now();

double time_now()
{
    struct timeval tv = tv_now();
    return (tv.tv_sec - tv_start.tv_sec) + 1e-6*(tv.tv_usec - tv_start.tv_usec);
}
