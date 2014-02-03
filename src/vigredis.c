#include "dict.h"
#include "util.h"
#include<stdio.h>

#define SIZE VR_DICT_INIT_SIZE
int main()
{

    dict d;
    dict_init(&d);
    char key[14];
    unsigned long i;
    
    for(i=0;i< SIZE*1000;i++)
    {
        //gen_random(keys[i],100);
        //gen_random(values[i],100);
        sprintf(key,"hello %07d",(int)i);
        //printf("%s\n",key);
        dict_add(&d,key,13,"value",5,VR_FLAG_NONE);
        //if( (i%1000) == 0)
        //    printf("i = %ld\n",i);
    }
    
    //dict_print(&d);
    return 0;
}
