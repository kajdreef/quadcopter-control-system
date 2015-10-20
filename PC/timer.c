#include "timer.h"
#include <stdio.h>


/*------------------------------------------------------------------
 * set_start_time -- Set the start time of the timer
 * Input:
 *			Timer *t - The used timer
 *
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void set_start_time(struct Timer *t){
	struct timespec startTime;
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	t->start = startTime.tv_sec*NANO + startTime.tv_nsec;
}


/*------------------------------------------------------------------
 * set_current_time -- Set the current time of the timer
 * Input:
 *			Timer *t - The used timer
 *
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
void set_current_time(struct Timer *t){
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	t->stop = currentTime.tv_sec*NANO + currentTime.tv_nsec;
}



/*------------------------------------------------------------------
 * get_diff_time -- return the time difference
 * Input:
 *			Timer *t - The used timer
 * Return:
 *			long long - difference between the start and currentTime of
 *									the timer.
 *
 * Author: Kaj Dreef
 *------------------------------------------------------------------
 */
long long get_diff_time(struct Timer t){
  return (t.stop - t.start);
}
