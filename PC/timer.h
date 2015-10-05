#include <time.h>

#define NANO 1000000000L

struct Timer {
  long long start;
  long long stop;
};

void set_start_time(struct Timer *t);
void set_current_time(struct Timer *t);
long long int get_diff_time(struct Timer t);
