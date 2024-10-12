#ifndef DEBUG_H
#define DEBUG_H

#include <time.h>
#include "cvector.h"
#include "cvector_utils.h"

double get_average_time(cvector_vector_type(clock_t) times){
    cvector_iterator(clock_t) it;

    // Delete max and min measurements
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


#endif 


/*
#include <pthread.h>

struct context {
    cvector_vector_type(int) nums;
    cvector_iterator(int) start;
    cvector_iterator(int) stop;
    int* counter;
};

void* myThreadFun(void *vargp) { 
    struct context* ctx = vargp;
    cvector_iterator(int) it = ctx->start;
    for (; it < ctx->stop; it++){
        if(*it == 333){
            *(ctx->counter) += 1;
        }
    }
    return NULL; 
} 

//890 базовый for для 1 000 000 элементов
//3582 мультитред 10
//1732 мультитред 1

int main(int argc, char* argv[]) {
    int N = 1000000;
    int J = 2;


    cvector_vector_type(int) nums = NULL;
    cvector_vector_type(pthread_t) threads = NULL;
    cvector_vector_type(struct context) contexts = NULL;
    cvector_reserve(nums,N);
    cvector_reserve(threads,J);
    cvector_reserve(contexts,J);
    cvector_set_size(nums,N);
    cvector_iterator(int) it;
    int counter = 0;

    clock_t begin = clock();

    for(int i=0; i < J; i++){
        cvector_push_back(threads,0);
        cvector_push_back(contexts,
            ((struct context){
                .nums=nums,
                .counter=&counter,
                .start=&nums[(cvector_size(nums)/J)*i],
                .stop=&nums[(cvector_size(nums)/J)*(i+1)]
            })
        );
        pthread_create(cvector_back(threads), NULL, myThreadFun, cvector_back(contexts)); 
    }

    for(int i=0; i < J; i++){
        pthread_join(threads[i], NULL); 
    }    

    clock_t end = clock();
    printf("time=%lu\n",end-begin);
    printf("counter=%d\n",counter);



*/
/*
    cvector_for_each_in(it, nums){
        if(*it == 333){
            counter += 1;
        }
    }
    */