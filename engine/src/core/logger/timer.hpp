#ifndef __TIMER_UTILITY__
#define __TIMER_UTILITY__

#include <chrono>

#define TIME_IN_MILLISECONDS(x) (std::chrono::duration_cast<std::chrono::milliseconds>(x).count())
#define TIME_IN_MICROSECONDS(x) (std::chrono::duration_cast<std::chrono::microseconds>(x).count())
#define CURRENT_TIME_POINT std::chrono::high_resolution_clock::now()
#define CURRENT_TIME_SINCE_EPOCH CURRENT_TIME_POINT.time_since_epoch()
// #define CURRENT_TIME ((float)TIME_IN_MICROSECONDS(CURRENT_TIME_SINCE_EPOCH) * 0.001f)
#define CURRENT_TIME TIME_IN_MILLISECONDS(CURRENT_TIME_SINCE_EPOCH)




/*

How to

int start_time=CURRENT_TIME

{
function

}

int end_time=CURRENT_TIME;

int duration=end_time-start_time;

*/



#endif