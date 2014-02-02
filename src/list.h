/*
 * A good ol linked list implementations
 */
#ifndef __LIST_H__
#define __LIST_H__

#define FLAG_NONE 0
#define FLAG_NX 1
#define FLAG_XX 2

// Key Exists
#define ERR_EXIST 1
#define ERR_OK 0
//Key does not exist
#define ERR_NOTEXIST 2

typedef struct list_node
{
    struct list_node* next;
    char* key;
    char* value;
    long klen;
    long vlen;
} list_node;

typedef struct list
{
    long len;
    list_node* root;
} list;

void list_init(list *l);
list_node* list_find(list *l,char* key,int klen);
int list_add(list* l,char* key,int klen,char*value,int vlen,int flag);
int list_delete(list *l,char* key,int klen);
void list_print(list *l);

#endif