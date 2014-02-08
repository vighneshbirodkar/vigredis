#include "dict.h"
#include "util.h"
#include "skip_list.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>


#define SIZE 1000
int main(int argc,char** argv)
{
    struct timespec start, stop;
    int i;
    int r;
    long range = atol(argv[1]);
    skip_list sl;
    int not_there = rand();
    skip_list_init(&sl);
    srand (10);
    
    for(i=0;i < range;i++)
    {
        r = rand();
        if(r != not_there)
            skip_list_insert(&sl,r);
    }
    
    clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &start);
    skip_list_insert(&sl,not_there);
    clock_gettime( CLOCK_PROCESS_CPUTIME_ID, &stop);
    printf("%d    %ld\n",range,stop.tv_nsec - start.tv_nsec);


    return 0;
}
