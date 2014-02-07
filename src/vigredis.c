#include "dict.h"
#include "util.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define SIZE 10000000
int main()
{

    dict d;
    dict_init(&d,VR_TYPE_STRING);
    static char key[SIZE][14];
    long i;

    for(i=0;i< SIZE;i++)
    {
        sprintf(key[i],"hello %07d",(int)i);
        dict_add_string(&d,key[i],13,"value",5,VR_FLAG_NONE);
    }

    for(i=(SIZE-1) ;i >= 0;i--)
    {
        if( dict_delete(&d,key[i],13) != VR_ERR_EXIST )
            printf("Something is wrong\n");
    }

    
    dict_print(&d);
    free(d.table);
    return 0;
}
