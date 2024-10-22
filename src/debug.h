#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG

#include <time.h>
#include "cvector.h"
#include "cvector_utils.h"

double get_average_time(cvector_vector_type(clock_t) times){
    cvector_iterator(clock_t) it;

    // Delete max and min measurements
    if ( cvector_size(times) > 10 ) {
        for(int i = 0 ; i < cvector_size(times)/100; i++){
            clock_t max = times[0];
            int maxpos = 0;
            clock_t min = times[0];
            int minpos = 0;

            cvector_for_each_in(it, times){
                if(*it > max){
                    max = *it;
                    maxpos = it - times;
                }
                if(*it < min){
                    min = *it;
                    minpos = it - times;
                }
            }
            cvector_erase(times,maxpos);
            cvector_erase(times,minpos);
        }
    }

    // Max and average search
    // max needed to median search
    //clock_t max = 0;
    double  average = 0;
    cvector_for_each_in(it, times){
        //if(*it > max){
        //    max = *it;
        //}
        average += *it;
    }
    average /= cvector_size(times);

    /*
    // median
    double sum = 0;
    cvector_for_each_in(it, times){
        double sub = 1.0 - (((double)*it - average)/(double)max);
        sum += *it * sub * sub * sub;
    }
    */

    return average;
    //printf("average=%f\n",average);
    //return sum / cvector_size(times);
}

double get_fastest_time(cvector_vector_type(clock_t) times){
    cvector_iterator(clock_t) it;

    clock_t max = 0;
    cvector_for_each_in(it, times){
        if(*it > max){
            max = *it;
        }
    }
    return max;
}

#define DEBUG_TIMEBLOCK_START(name) \
clock_t debug_time_start_##name = clock()

#define DEBUG_TIMEBLOCK_STOP(name)          \
clock_t debug_time_stop_##name = clock();   \
printf("DEBUG once "#name"=%lu\n", debug_time_stop_##name - debug_time_start_##name)

#define DEBUG_MULTIPLE_TIMEBLOCK_START(name,_times) \
{                                                   \
    cvector(clock_t) times = NULL;                  \
    cvector_reserve(times,_times);                  \
    unsigned int _i=0;                              \
    for(; _i < _times; _i++){                       \
        clock_t time_start = clock()


#define DEBUG_MULTIPLE_TIMEBLOCK_STOP(name)                 \
        clock_t time_stop = clock();                        \
        cvector_push_back(times, time_stop - time_start);   \
    }                                                       \
    double avg = get_average_time(times);                   \
    printf("DEBUG multiple "#name" n=%u t=%.2f\n", _i, avg);\
}

#else

#define DEBUG_TIMEBLOCK_START(name)
#define DEBUG_TIMEBLOCK_STOP(name)
#define DEBUG_MULTIPLE_TIMEBLOCK_START(name,_times)
#define DEBUG_MULTIPLE_TIMEBLOCK_STOP(name)

#endif

#endif
