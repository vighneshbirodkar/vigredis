/*
 * A dynamically resizing hash table implementation
 */

#ifndef __DICT_H__
#define __DICT_H__

#include "stdint.h"
#include "list.h"

#define VR_DICT_INIT_SIZE 1000


typedef struct dict 
{
    //Number of elements in dict
    uint32_t len;
    //The number of slots in the dict
    uint32_t size;
    list* table;
} dict;


void dict_init(dict* d);
int dict_add(dict *d,char *key,int klen,char* value,int vlen,int flag);
void dict_print(dict *d);
void dict_debug_print(dict *d);

#endif
