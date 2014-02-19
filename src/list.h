/*
 * A good ol linked list implementations
 */
#ifndef __LIST_H__
#define __LIST_H__

#include "stdint.h"
#include "vr_object.h"

#define VR_FLAG_NONE 0
#define VR_FLAG_NX 1
#define VR_FLAG_XX 2

// Key Exists
#define VR_ERR_EXIST -1
//In Short No Problemo
#define VR_ERR_OK 0
//Key does not exist
#define VR_ERR_NOTEXIST -2
//Something has gone seriously wrong
#define VR_ERR_FATAL -3

//The node is of type string
#define VR_TYPE_STRING 1

//The node is of type int
#define VR_TYPE_INT 2

typedef struct list_node
{
    struct list_node* next;
    int klen;
    char *key;
    vr_object object;
    long expiry;
} list_node;

typedef struct list
{
    char type;
    uint32_t len;
    list_node* root;
} list;

void list_init(list *l,char type);
list_node* list_find(list *l,char* key,int klen);
int list_add_object(list* l,char* key,int klen,vr_object object,int flag);
int list_add_string(list* l,char* key,int klen,char* value,int vlen,int flag);
int list_add_int(list *l,char *key,int klen,int value,int flag);
int list_delete_int(list *l,char* key,int klen);
int list_delete_string(list *l,char* key,int klen);
int list_delete_object(list *l,char* key,int klen);
void list_print(list *l);

#endif
