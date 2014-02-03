#ifndef __HASH_H__
#define __HASH_H__

/*
 * Convention foloowed for hash functions
 * hash_*data-type*_*output-bits*

*/
#include "stdint.h"


#define _DICT_SEED_ 0xa7b5437c

uint32_t hash_string_32(const char* string,int len);

#endif
