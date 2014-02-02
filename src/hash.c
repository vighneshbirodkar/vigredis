/* 
* hash.c - Contains hash function implementations
*/

#include "hash.h"
#include<stdio.h>

/*
 * A MurmurHash3 implementation
 * http://en.wikipedia.org/wiki/MurmurHash#Algorithm
 * 

 * Assumes a little endian machine 
 */
uint32_t hash_string_32(char* string,int len)
{
    uint32_t seed = _DICT_SEED_;
    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;
    uint32_t r1 = 15;
    uint32_t r2 = 13;
    uint32_t m = 5;
    uint32_t n =0xe6546b64;
    uint32_t k;
    uint32_t a1,a2,a3,a4;
    uint32_t remainingBytes = 0x00;

    uint32_t hash = seed;
    int i;
    int limit = len/4;

    for(i=0;i < limit ; i++)
    {
        a1 = string[4*i];
        a2 = string[4*i+1];
        a3 = string[4*i+2];
        a4 = string[4*i+3];

        k = a1 + (a2 << 8) + (a3 << 16) + (a4 << 24);
        
        k = k*c1;
        k = (k << r1);
        k = k*c2;

        hash = hash ^ k;
        hash = (hash << r2 );
        hash = hash*m + n;
    }
    
    switch(len % 4)
    {
        case 0:
            break;
        case 1 :
            remainingBytes = string[4*limit];
            break;
        case 2 :
            remainingBytes = string[4*limit ] + (string[4*limit + 1] << 8 );
            break;
        case 3 :
            remainingBytes = string[4*limit ] + (string[4*limit + 1] << 8 ) + (string[4*limit + 2] << 16 );
            break;
    }
    
    remainingBytes *= c1;
    remainingBytes = (remainingBytes << r1);
    remainingBytes*= c2;
    hash = hash ^ remainingBytes;
    
    hash = hash ^ len;

    hash = hash ^ (hash >> 16);
    hash = hash * 0x85ebca6b;
    hash = hash ^ ( hash >> 13);
    hash = hash * 0xc2b2ae35;
    hash = hash ^ ( hash >> 16);
    
    return hash;
}
