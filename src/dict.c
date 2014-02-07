/*
 * A dynamically resizing hash table implementation
 */

#include "dict.h"
#include "hash.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>


void dict_init(dict* d,char type)
{
    uint32_t i;
    d->len = 0;
    d->size = VR_DICT_INIT_SIZE;
    d->table = (list*)malloc(sizeof(list)*VR_DICT_INIT_SIZE);
    d->type = type;
    for(i=0;i < d->size ;i ++)
        list_init(&d->table[i],type);
}


int dict_add_int(dict *d,char *key,int klen,int value,int flag)
{
    if(d->type != VR_TYPE_INT)
    {
        printf("Incompatible List Addition\n");
        return VR_ERR_FATAL;
    }
    
    vr_object obj;
    obj.value = value;
    return dict_add_object(d,key,klen,obj,flag);
    
}


int dict_add_string(dict *d,char* key,int klen,char* value,int vlen,int flag)
{
    if(d->type != VR_TYPE_STRING)
    {
        printf("Incompatible List Addition\n");
        return VR_ERR_FATAL;
    }
    
    
    vr_object obj;
    obj.string.len = vlen;
    obj.string.string = (char*)malloc(sizeof(char)*vlen);
    strncpy(obj.string.string,value,vlen);
    return dict_add_object(d,key,klen,obj,flag);
}


int dict_add_object(dict *d,char* key,int klen,vr_object object,int flag)
{
    uint32_t hash = hash_string_32(key,klen);
    uint32_t index = hash % d->size;
    int ret_val;
    
    ret_val = list_add_object(&d->table[index],key,klen,object,flag);
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
    
    ret_val = list_delete_object(&d->table[index],key,klen);
    
    //printf("Delting %.*s from SLOT %d\n",klen,key,index);
    //printf("err del = %d len = %d\n",ret_val,(int)d->len);
    if( ret_val == VR_ERR_EXIST)
        d->len--;
        
    if(d->len < (d->size/VR_DICT_CONTRACT_RATIO))
        dict_contract(d);
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
        list_init(&d->table[i],d->type);
    
    d->len = 0;
    
    
    for(i=0;i < old_size;i++)
    {
        current = old_table[i].root;
        while(current != NULL)
        {
            tmp = current;
            current = current->next;
            dict_add_object(d,tmp->key,tmp->klen,tmp->object,VR_FLAG_NONE);
            //printf("Added : %.*s\n", (int)tmp->klen, tmp->key );
            free(tmp->key);
            free(tmp);
        }
    
    }
    free(old_table);
    
}


void dict_contract(dict *d)
{
    list* old_table = d->table;
    uint32_t old_size = d->size;
    uint32_t i;
    uint32_t new_size = VR_DICT_INIT_SIZE;
    list_node *tmp,*current;
    
    //printf("Contract\n");
    while(new_size < d->len)
        new_size *= 2;
    
    //new_size = VR_MAX(VR_DICT_INIT_SIZE,new_size);
    
    d->table = (list*)malloc(sizeof(list)*new_size);
    d->size = new_size;
    d->len = 0;
    
    for(i=0;i < d->size ;i ++)
        list_init(&d->table[i],d->type);
    
    //printf("Starting Loop\n");
    for(i=0;i < old_size;i++)
    {
        current = old_table[i].root;
        while(current != NULL)
        {
            tmp = current;
            current = current->next;
            //printf("Staring to Add\n");
            dict_add_object(d,tmp->key,tmp->klen,tmp->object,VR_FLAG_NONE);
            //printf("Added : %.*s\n", (int)tmp->klen, tmp->key );
            free(tmp->key);
            free(tmp);
        }
    
    }
    free(old_table);
    //printf("Contract Over\n");
    
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
            if(d->type == VR_TYPE_STRING)
                printf("%.*s : %.*s\n", (int)tmp->klen, tmp->key , (int)tmp->object.string.len, tmp->object.string.string);
            if(d->type == VR_TYPE_INT)
                printf("%.*s : %.*d\n", (int)tmp->klen, tmp->key , tmp->object.values);
            tmp = tmp->next;
        }
    }

}




