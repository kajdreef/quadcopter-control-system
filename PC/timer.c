#include <stdio.h>
#include "timer.h"

void set_start_time(struct Timer *t){
	struct timespec startTime;
	clock_gettime(CLOCK_MONOTONIC, &startTime);
	t->start = startTime.tv_sec*NANO + startTime.tv_nsec;
}

void set_current_time(struct Timer *t){
	struct timespec currentTime;
	clock_gettime(CLOCK_MONOTONIC, &currentTime);
	t->stop = currentTime.tv_sec*NANO + currentTime.tv_nsec;
}

long long get_diff_time(struct Timer t){
  return (t.stop - t.start);
}
