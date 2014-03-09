/*
 * A generic object
 * It can hold either an int,a string or a pointer
 */
 
 
#ifndef __VR_OBJECT_H__
#define __VR_OBJECT_H__

#include "vr_string.h"
//#include "set.h"
 
struct set;

typedef union vr_object
{
    double value;
    struct set* obj_set;
    struct vr_string string;
    
} vr_object;

#endif
