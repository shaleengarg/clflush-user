/*
 * Will allocate a data structure  with size  = x * page size and
 * read it repeatedly, time the access latency before and after clflush
 */
#include <sys/time.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <fcntl.h>
#include <inttypes.h>

#ifndef __USE_GNU
#define __USE_GNU
#endif
#define _GNU_SOURCE

#include <sched.h>

#include <asm/unistd.h>

#define MB    (1024*1024)
#define KB    1024
#define PAGESIZE (4 * KB)
#define ITER 100
#define NRPAGES 10
#define PAGESHIFT 12

/*
 * function that flushes TLB entry for the page
 * associated with this pointer
 */
static void do_flush_cache(volatile void *p) {
	asm volatile ("clflush (%0)" :: "r"(p));
}

void tlb_flush_penalty(void){

    struct timespec tps, tpe;

    int nr_ints_one_page = (1 << PAGESHIFT)/sizeof(int);
    printf("nr of ints %d\n", nr_ints_one_page);
    int nr_ints = (NRPAGES << PAGESHIFT)/sizeof(int);
    int  *testarr = (int *)malloc(nr_ints * sizeof(int));
    
    int i;

    clock_gettime(CLOCK_REALTIME, &tps);
    for(i=0; i<nr_ints*ITER; i+=nr_ints_one_page)
    {
        testarr[i%nr_ints] = i;
        //printf("testarr %d\n", i);
    }
    clock_gettime(CLOCK_REALTIME, &tpe);

    double timespent = ((tpe.tv_sec-tps.tv_sec)*1000000000 + 
            tpe.tv_nsec-tps.tv_nsec)/1000;

    printf("General timepent without %lf\n", timespent);


    clock_gettime(CLOCK_REALTIME, &tps);
    for(i=0; i<(nr_ints*ITER); i+=nr_ints_one_page)
    {
        testarr[i%nr_ints] = i;
        //printf("testarr %d\n", i);
        do_flush_cache(&testarr[i]);
    }
    clock_gettime(CLOCK_REALTIME, &tpe);

    timespent = ((tpe.tv_sec-tps.tv_sec)*1000000000 + 
            tpe.tv_nsec-tps.tv_nsec)/1000;

    printf("General timepent with flush %lf\n", timespent);

}

int main(int argc, char** argv){

    tlb_flush_penalty();
    return 0;
}
