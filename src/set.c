/*
 * an ordered set implementation
 * each element is stores in the sets dictionary as well as its skip_list
 */

#include"set.h"
#include "dict.h"
#include"skip_list.h"
#include <stdlib.h>
#include <stdio.h>
#include "util.h"

/*
 * initialization
 */
void set_init(set *s)
{
    dict_init(&s->set_dict,VR_TYPE_DOUBLE);
    skip_list_init(&s->set_list);
    s->len = 0;
}

/*
 * adds `key` with `score` to `s`
 * if `key` existed its score is updated
 * returns
 * VR_ERR_OK if member was absent
 * VR_ERR_EXIST if member was present
 */

int set_add(set *s,char* key,int klen,double score)
{
    int ret;
    double exp;
    vr_object *obj;


    obj = dict_get(&s->set_dict,key,klen,&exp);
    if( obj == NULL)
    {
        ret = dict_add_double(&s->set_dict,key,klen,score,VR_FLAG_NONE,-1);
        skip_list_insert(&s->set_list,score,key,klen);
        if(ret != VR_ERR_OK)
            printf("Fatal : Key which is not supposed to exists, does exist\n");
        s->len = s->len + 1;
        //printf("set len = %d value = %lf\n",s->len,score);
    }
    else
    {
        if(VR_EQ(score,obj->value))
            return VR_ERR_EXIST;
        skip_list_delete_with_key(&s->set_list,obj->value,key,klen);
        ret = dict_add_double(&s->set_dict,key,klen,score,VR_FLAG_NONE,-1);
        skip_list_insert(&s->set_list,score,key,klen);
        if(ret != VR_ERR_EXIST)
            printf("Fatal : Key which is supposed to exist, does not exist\n");
    }
    return ret;
}

void set_print(set* s)
{
    skip_list_print(&s->set_list);
}

void set_clear(set *s)
{
    dict_clear(&s->set_dict);
    skip_list_clear(&s->set_list);
}


