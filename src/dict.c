/*
 * A dynamically resizing hash table implementation
 */

#include "dict.h"
#include "hash.h"
#include<stdlib.h>
#include<stdio.h>


void dict_init(dict* d)
{
    d->len = 0;
    d->size = VR_DICT_INIT_SIZE;
    d->table = (list*)malloc(sizeof(list)*VR_DICT_INIT_SIZE);
}

int dict_add(dict *d,char *key,int klen,char* value,int vlen,int flag)
{
    uint32_t hash = hash_string_32(key,klen);
    uint32_t index = hash % d->size;
    int ret_val;
    
    ret_val = list_add(&d->table[index],key,klen,value,vlen,flag);
    if( ret_val == VR_ERR_OK)
        d->len++;
    return ret_val;
}

void dict_print(dict *d)
{
    uint32_t i;
    list_node* tmp ;
    
    printf("Length = %u\n",d->len);
    for(i=0; i<d->size; i++)
    {
        tmp = (&d->table[i])->root;
        while( tmp != NULL)
        {
            printf("%.*s : %.*s\n", (int)tmp->klen, tmp->key , (int)tmp->vlen, tmp->value);
            tmp = tmp->next;
        }
    }

}

void dict_debug_print(dict *d)
{
    uint32_t i;
    
    printf("Dict Length = %u\n",d->len);
    for(i=0; i<d->size; i++)
    {
        printf("----------SLOT %u---------\n",i);
        list_print(&d->table[i]);
    }
}


