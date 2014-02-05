#include "dict.h"
#include "util.h"
#include<stdio.h>
#include<stdlib.h>


#define SIZE 1000000
int main()
{

    /*dict d;
    dict_init(&d);
    static char key[SIZE][14];
    long i;
    
    for(i=0;i< SIZE;i++)
    {
        sprintf(key[i],"hello %07d",(int)i);
        dict_add_string(&d,key[i],13,"value",5,VR_FLAG_NONE);
    }

    for(i=(SIZE-1) ;i >= 0;i--)
    {
        if( dict_delete(&d,key[i],13) != VR_ERR_OK )
            printf("Something is wrong\n");
    }

    
    //dict_print(&d);
    //printf("new size = %u\n",d.size);
    
    free(d.table);
*/
    
    list l;
    list_init(&l,VR_TYPE_INT);
    list_add_int(&l,"ab1",3,3,VR_FLAG_NONE);
    list_add_int(&l,"ab2",3,5,VR_FLAG_NONE);
    list_add_int(&l,"ab3",3,8,VR_FLAG_NONE);
    list_add_int(&l,"ab4",3,9,VR_FLAG_NONE);
    list_add_int(&l,"ab5",3,10,VR_FLAG_NONE);
    
    //list_delete_int(&l,"ab1",3);
    list_delete_int(&l,"ab2",3);
    //list_delete_int(&l,"ab3",3);
    list_delete_int(&l,"ab4",3);
    //list_delete_int(&l,"ab5",3);
    list_print(&l);

    return 0;
}
