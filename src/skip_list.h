
#ifndef __SKIP_LIST_H__
#define __SKIP_LIST__

#include "vr_string.h"

#define VR_SKIP_LIST_MAX_NODE 32

typedef struct skip_list_node
{
    double score;
    struct vr_string string;
    struct skip_list_node** next;
} skip_list_node;

typedef struct skip_list 
{
    struct skip_list_node* header;
    int level;
} skip_list;

void skip_list_init(skip_list *sl);
void skip_list_insert(skip_list *l,double);
int random_level();
void skip_list_print(skip_list* sl);


#endif
