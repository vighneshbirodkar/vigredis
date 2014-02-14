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

#endif
