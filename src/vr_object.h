/*
 * A generic object
 * It can hold either an int,a string or a pointer
 */
 
 
#ifndef __VR_OBJECT_H__
#define __VR_OBJECT_H__

#include "vr_string.h"
 
typedef union vr_object
{
    int value;
    struct vr_string string;
    
} vr_object;

#endif
