/*
 * A generic object
 * It can hold either an int,a string or a pointer to set
 */
 
 
#ifndef __VR_OBJECT_H__
#define __VR_OBJECT_H__

#include "vr_string.h"
//#include "set.h"
 
struct set;

//the object type used in dict
typedef union vr_object
{
    double value;
    struct set* obj_set;
    struct vr_string string;
    
} vr_object;

#endif
