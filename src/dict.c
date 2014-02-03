/*
 * A dynamically resizing hash table implementation
 */

#include "dict.h"
#include "hash.h"
#include<stdlib.h>
#include<stdio.h>


void dict_init(dict* d)
{
    uint32_t i;
    d->len = 0;
    d->size = VR_DICT_INIT_SIZE;
    d->table = (list*)malloc(sizeof(list)*VR_DICT_INIT_SIZE);
    for(i=0;i < d->size ;i ++)
        list_init(&d->table[i]);
}

int dict_add(dict *d,char *key,int klen,char* value,int vlen,int flag)
{
    uint32_t hash = hash_string_32(key,klen);
    uint32_t index = hash % d->size;
    int ret_val;
    
    ret_val = list_add(&d->table[index],key,klen,value,vlen,flag);
    if( ret_val == VR_ERR_OK)
        d->len++;
        
    if(d->len > d->size)
        dict_expand(d);
    return ret_val;
}


int dict_delete(dict *d,char *key,int klen)
{
    uint32_t hash = hash_string_32(key,klen);
    uint32_t index = hash % d->size;
    int ret_val;
    
    ret_val = list_delete(&d->table[index],key,klen);
    
    //printf("Delting %.*s from SLOT %d\n",klen,key,index);
    if( ret_val == VR_ERR_OK)
        d->len--;
    return ret_val;
}

void dict_expand(dict *d)
{
    list* old_table = d->table;
    uint32_t old_size = d->size;
    uint32_t i;
    
    
    list_node *tmp,*current;
    
    d->table = (list*)malloc(sizeof(list)*old_size*VR_DICT_EXPAND_RATIO);
    d->size = old_size*VR_DICT_EXPAND_RATIO;
    
    for(i=0;i < d->size ;i ++)
        list_init(&d->table[i]);
    
    d->len = 0;
    

    
    for(i=0;i < old_size;i++)
    {
        current = old_table[i].root;
        while(current != NULL)
        {
            tmp = current;
            current = current->next;
            dict_add(d,tmp->key,tmp->klen,tmp->value,tmp->vlen,VR_FLAG_NONE);
            //printf("Added : %.*s\n", (int)tmp->klen, tmp->key );
            free(tmp->key);
            free(tmp->value);
            free(tmp);
        }
    
    }
    free(old_table);
    
    
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


