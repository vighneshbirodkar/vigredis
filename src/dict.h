/*
 * A dynamically resizing hash table implementation
 */

#ifndef __DICT_H__
#define __DICT_H__

#include "stdint.h"
#include "list.h"
#include "skip_list.h"

#define VR_DICT_INIT_SIZE 8
#define VR_DICT_EXPAND_RATIO 2
#define VR_DICT_CONTRACT_RATIO 10


typedef struct dict 
{
    //Number of elements in dict
    uint32_t len;
    //The number of slots in the dict
    uint32_t size;
    char type;
    list* table;
} dict;


void dict_init(dict* d,char type);
int dict_add_string(dict *d,char* key,int klen,char* value,int vlen,int flag,double expiry);
int dict_add_object(dict *d,char *key,int klen,vr_object object,int flag,double expiry);
int dict_delete(dict *d,char *key,int klen);
vr_object* dict_get(dict *d,char* key,int klen,double* expiry);
vr_string* dict_get_string(dict* d,char* key,int klen,double * expiry);
void dict_clear(dict *d);
void dict_expand(dict *d);
void dict_contract(dict *d);
void dict_print(dict *d);
void dict_debug_print(dict *d);
int dict_set_bit(dict *d,char* key,int klen,int n,char b);
int dict_get_bit(dict *d,char* key,int klen,int n);
void dict_delete_expired(dict *d,skip_list* sl);


#endif
