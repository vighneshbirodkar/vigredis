/*
 * A dynamically resizing hash table implementation
 */

#ifndef __DICT_H__
#define __DICT_H__

#include "stdint.h"
#include "list.h"

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
int dict_add_string(dict *d,char* key,int klen,char* value,int vlen,int flag);
int dict_add_object(dict *d,char *key,int klen,vr_object object,int flag);
int dict_delete(dict *d,char *key,int klen);
vr_object* dict_get(dict *d,char* ket,int klen);
void dict_clear(dict *d);
void dict_expand(dict *d);
void dict_contract(dict *d);
void dict_print(dict *d);
void dict_debug_print(dict *d);


#endif
