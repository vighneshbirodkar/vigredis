
#ifndef __SKIP_LIST_H__
#define __SKIP_LIST_H__

#include "vr_string.h"

#define VR_SKIP_LIST_MAX_NODE 32

typedef struct skip_list_node
{
    double score;
    struct vr_string string;
    struct skip_list_node** next;
    char* key;
    int klen;
} skip_list_node;

typedef struct skip_list 
{
    struct skip_list_node* header;
    int level;
} skip_list;

void skip_list_init(skip_list *sl);
void skip_list_insert(skip_list *sl,double score,char* key,int klen);
int random_level();
void skip_list_print(skip_list* sl);
double skip_list_first(skip_list* sl);
int skip_list_empty(skip_list* sl);
skip_list_node* skip_list_pop(skip_list *sl);
void skip_list_clear(skip_list* sl);

#endif
