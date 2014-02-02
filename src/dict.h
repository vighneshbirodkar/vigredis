/*
 * A dynamically resizing hash table implementation
 */

#ifndef __HASH_H__
#define __HASH_H__

#include "list.h"


struct dict 
{
    long len;
    long size;
    
    list* table;
} dict;

#endif
