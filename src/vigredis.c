#include "dict.h"
#include "util.h"
#include<stdio.h>

int main()
{

    long i;
    list l;
    
    list_init(&l);
    char str[1000][10];
    for(i=0;i<1000;i++)
    {
        gen_random(str[i],9);
        list_add(&l,str[i],10,str[i],10,VR_FLAG_NONE);
    }
    
    for(i=0;i<100;i++)
    {
        //gen_random(str[i],9);
        list_delete(&l,str[i],10);
    }
    list_print(&l);
    return 0;
}
