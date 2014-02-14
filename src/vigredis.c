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
    dict d;
    int i;
    dict_init(&d,VR_TYPE_STRING);
    char key[10] = {0};
    for(i=0;i<SIZE;i++)
    {
        sprintf(key,"%9d",i);
        dict_add_string(&d,key,10,"hello",5,VR_FLAG_NONE);
    }
    
    /*for(i=0;i<SIZE;i++)
    {
        sprintf(key,"%9d",i);
        dict_delete(&d,key,10);
    }
    
    free(d.table);
    */
    dict_clear(&d);
    return 0;
}
