/******************************************************
 *
 * Author: Christoph Bodenstein
 * Date: 2007
 *
 * Some functions for time measurement 
 *******************************************************/

#include <time.h>
#include <sys/time.h>
#include "timing.h"

double seconds(void) {
	struct timezone tz;
	struct timeval t;
	gettimeofday(&t, &tz);
	return (double) t.tv_sec + ((double)t.tv_usec/1e6);
}

unsigned long long getTimeTicks() {
	unsigned long long tbr;
__asm__	__volatile__("mftb %[tbr]" : [tbr] "=r" (tbr):);
	return tbr;
}

