/*
 * A dynamically resizing hash table implementation
 */

#include "dict.h"
#include "hash.h"
#include "util.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"vr_string.h"
#include "skip_list.h"

/*
 * Initialization
 */
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

/*
 * adds an int to dict,see dic_add_object
 */
int dict_add_int(dict *d,char *key,int klen,int value,int flag,double expiry)
{
    if(d->type != VR_TYPE_INT)
    {
        printf("Incompatible Dict Addition\n");
        return VR_ERR_FATAL;
    }
    
    vr_object obj;
    obj.value = value;
    return dict_add_object(d,key,klen,obj,flag,expiry);
    
}

/*
 * adds an string to dict,see dic_add_object
 */
int dict_add_string(dict *d,char* key,int klen,char* value,int vlen,int flag,double expiry)
{
    if(d->type != VR_TYPE_STRING)
    {
        printf("Incompatible Dict Addition\n");
        return VR_ERR_FATAL;
    }
    
    vr_object obj;
    obj.string.len = vlen;
    obj.string.string = (char*)malloc(sizeof(char)*vlen);
    strncpy(obj.string.string,value,vlen);
    return dict_add_object(d,key,klen,obj,flag,expiry);
}

/*
 * gets bit at specified offset of key
 * if key doesnt exist, return 0
 */
int dict_get_bit(dict *d,char* key,int klen,int n)
{
    list_node* tmp;
    uint32_t hash = hash_string_32(key,klen);
    uint32_t index = hash % d->size;
    if(d->type != VR_TYPE_STRING)
    {
        printf("Incompatible Dict Operation\n");
        return VR_ERR_FATAL;
    }
    
    tmp = list_find(&d->table[index],key,klen);
    
    if(tmp)
        return string_get_bit(&tmp->object.string,n);
    else
        return 0;
}

/*
 * gets bit at specified offset of key
 * if key doesnt exist, its added with all 0s, and bit is set
 */

int dict_set_bit(dict *d,char* key,int klen,int n,char b)
{
    list_node* tmp;
    uint32_t hash = hash_string_32(key,klen);
    uint32_t index = hash % d->size;
    if(d->type != VR_TYPE_STRING)
    {
        printf("Incompatible Dict Operation\n");
        return VR_ERR_FATAL;
    }
    
    tmp = list_find(&d->table[index],key,klen);
    
    if(tmp)
        return string_set_bit(&tmp->object.string,n,b);
    else
    {
        vr_object obj;
        obj.string.string = NULL;
        obj.string.len = 0;
        dict_add_object(d,key,klen,obj,VR_FLAG_NONE,-1);
        return string_set_bit(&obj.string,n,b);
        
    }
}

/*
 * Adds a key, value pair in the dict.
 * 
 * if flag = VR_FLAG_NONE
 *      values of existing keys are replaced
 * if flag = VR_FLAG_NX
 *      values are only added if key exists
 * if flag = VR_FLAG_XX
 *      values are replaced only if keys exist
 *
 * returns 
 * VR_ERR_OK - if key didn't exist
 * VR_ERR_EXIST - if key existed
 * VR_ERR_NOTEXIST - if key did not exist and flag = VR_FLAG_XX
 */
int dict_add_object(dict *d,char* key,int klen,vr_object object,int flag,double expiry)
{
    uint32_t hash = hash_string_32(key,klen);
    uint32_t index = hash % d->size;
    int ret_val;
    
    ret_val = list_add_object(&d->table[index],key,klen,object,flag,expiry);
    if( ret_val == VR_ERR_OK)
        d->len++;
        
    if(d->len > d->size)
        dict_expand(d);
    return ret_val;
}

/*
 * Deletes a key from dict,
 * returns 
 * VR_ERR_EXIST if key existed
 * VR_NOT_EXIST if key didnt exist
 */
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

/*
 * Deletes a key from dict,only if it has expired
 * time is assumed to be as given
 * returns 
 * VR_ERR_EXIST if key existed
 * VR_NOT_EXIST if key didnt exist
 */
int dict_delete_ife(dict *d,char *key,int klen, double time)
{
    uint32_t hash = hash_string_32(key,klen);
    uint32_t index = hash % d->size;
    int ret_val;
    
    ret_val = list_delete_object_ife(&d->table[index],key,klen,time);
    
    if(ret_val == VR_ERR_EXIST)
        d->len--;
        
    if(d->len < (d->size/VR_DICT_CONTRACT_RATIO))
        dict_contract(d);
    return ret_val;
}





/*
 * Increases size by VR_DICT_CONTRACT_RATIO times
 */
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
            dict_add_object(d,tmp->key,tmp->klen,tmp->object,VR_FLAG_NONE,tmp->expiry);
            //printf("Added : %.*s\n", (int)tmp->klen, tmp->key );
            free(tmp->key);
            free(tmp);
        }
    
    }
    free(old_table);
    
}

/*
 * Contracts dict to least size to hold all elements such that size is a 
 * multiple of 2
 */
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
            dict_add_object(d,tmp->key,tmp->klen,tmp->object,VR_FLAG_NONE,tmp->expiry);
            //printf("Added : %.*s\n", (int)tmp->klen, tmp->key );
            free(tmp->key);
            free(tmp);
        }
    
    }
    free(old_table);
    //printf("Contract Over\n");
    
}

/*
 * displays contents of dict
 */
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
                printf("%.*s : %d\n", (int)tmp->klen, tmp->key , (int)tmp->object.value);
            tmp = tmp->next;
        }
    }

}

/*
 * returns vr_string pointer to string pointed by key
 * NULL is key didn't exist
 */
vr_string* dict_get_string(dict* d,char* key,int klen,double * expiry)
{
    if(d->type != VR_TYPE_STRING)
    {
        printf("Incompatible Dict Addition\n");
        return NULL;
    }
    return &dict_get(d,key,klen,expiry)->string;
}


/*
 * reutrns object pointer for key, NULL if key doesn't exist
 * modifies expiry to expiry time
 */
vr_object* dict_get(dict *d,char* key,int klen,double* expiry)
{
    uint32_t hash = hash_string_32(key,klen);
    uint32_t index = hash % d->size;
    list_node* tmp;
    
    tmp = list_find(&d->table[index],key,klen);
    if(tmp)
    {
        *expiry = tmp->expiry;
        return &tmp->object;
    }
    else
        return NULL;

}

/*
 * deleted keys from d whose time has expired
 * keys are suumed to be stored in 'sl' in increasing order of expiry time
 */
void dict_delete_expired(dict *d,skip_list* sl)
{
    double time = get_time_ms();
    skip_list_node* tmp;
    
    while(  (time > skip_list_first(sl) ))
    {
        //printf("time = %lf\n",time);
        //printf("first = %lf\n",skip_list_first(sl));
        tmp = skip_list_pop(sl);
        dict_delete_ife(d,tmp->key,tmp->klen,time);

            //printf("deleted %.*s\n",tmp->klen,tmp->key);
        free(tmp->key);
        free(tmp->next);
        free(tmp);
        
    }
}
/*
 * deletes the dict, freeing all memory used
 */
void dict_clear(dict *d)
{
    list_node* tmp,*current;
    int i;
    for(i=0;i < d->size;i++)
    {
        current = d->table[i].root;
        while(current != NULL)
        {
            tmp = current;
            current = current->next;
            free(tmp->key);
            if(d->type == VR_TYPE_STRING)
            {
                free(tmp->object.string.string);
            }
            free(tmp);
        }
    
    }
    free(d->table);
}


