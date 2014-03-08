/*
 * A set implementation, each set member has an associated score
 * Sets can be ordered via this score
 */

#ifndef __SET_H__
#define __SET_H__

#include"set.h"
#include "dict.h"
#include"skip_list.h"

typedef struct set
{
    dict set_dict;
    skip_list set_list;
    int len;
} set;

void set_init(set *s);
void set_print(set* s);
void set_clear(set *s);
void set_add(set *s,char* key,int klen,double score);

#endif
