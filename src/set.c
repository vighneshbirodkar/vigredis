#include"set.h"
#include "dict.h"
#include"skip_list.h"
#include <stdlib.h>
#include <stdio.h>

void set_init(set *s)
{
    dict_init(&s->set_dict,VR_TYPE_DOUBLE);
    skip_list_init(&s->set_list);
    s->len = 0;
}

void set_add(set *s,char* key,int klen,double score)
{
    int ret;
    double exp;
    vr_object *obj;


    obj = dict_get(&s->set_dict,key,klen,&exp);
    if( obj == NULL)
    {
        ret = dict_add_double(&s->set_dict,key,klen,score,VR_FLAG_NONE,-1);
        if(ret != VR_ERR_OK)
            printf("Fatal : Key which is not supposed to exists, does exist\n");
    }
    else
    {
        skip_list_delete_with_key(&s->set_list,obj->value,key,klen);
        ret = dict_add_double(&s->set_dict,key,klen,score,VR_FLAG_NONE,-1);
        if(ret != VR_ERR_EXIST)
            printf("Fatal : Key which is supposed to exist, does not exist\n");
    }
}

void set_print(set* s)
{
    skip_list_print(&s->set_list);
}


