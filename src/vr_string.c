
#include "vr_string.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

/*
 * reutrns bit at 'n'
 * if 'n' exceeds length, returns 0
 */
int string_get_bit(vr_string* s,int n)
{
    int index,offset;
    char mask;
    char *str = s->string;
    
    if(n > s->len*8)
    {
        return 0;
    }
    else
    {
        index = n/8;
        offset = (n%8);
        mask = 0x80 >> offset;
        if(str[index] & mask)
            return 1;
        else
            return 0;
    }
}

/*
 * sets bit at 'n'
 * if 'n' exceeds length, string is expanded
 */

int string_set_bit(vr_string* s,int n,char bit)
{
    int index,offset;
    char mask,value;
    char* str = s->string;
    char *new_str;
    int old_val;
    
    if ( n < s->len*8)
    {
        index = n/8;
        offset = (n%8);
        mask = 0x80 >> offset;
        value = (bit*0x80) >> offset;
        old_val = str[index] & mask;
        str[index] = (str[index] & ~mask) | (value & mask) ;
    }
    else
    {
        index = n/8;
        new_str = (char*)malloc(index+1);
        bzero(new_str,index+1);
        strncpy(new_str,str,s->len);
        if(str != NULL)
            free(str);
        s->string = new_str;
        s->len = index + 1;
        str = s->string;
        
        index = n/8;
        offset = (n%8);
        mask = 0x80 >> offset;
        value = (bit*0x80) >> offset;
        old_val = str[index] & mask;
        str[index] = (str[index] & ~mask) | (value & mask) ;
    }
    if (old_val)
        return 1;
    else
        return 0;
}
