/*
 * A useful wrapper around char*
 */

#ifndef __VR_STRING_H__
#define __VR_STRING_H__

typedef struct vr_string 
{
    int len;
    char* string;
} vr_string;


int string_get_bit(vr_string* s,int n);
int string_set_bit(vr_string* s,int n,char bit);

#endif
