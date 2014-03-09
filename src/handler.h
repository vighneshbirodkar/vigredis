#ifndef __HANDLER_H__
#define __HANDLER_H__

#define VR_ERR_TOK 7
#include "dict.h"

#define VR_MAX_MSG_LEN 1024
#define VR_BUFFER_LEN 1024

typedef struct client_info
{
    char* buffer;
    int fd;
    int index;
    struct client_info *next,*prev;
} client_info;

typedef struct client_list 
{
    client_info* header;
}client_list;

void client_list_init(client_list *l);
void client_list_add(client_list* l,int fd);
void client_handle(client_info* client,dict* kv_dict,skip_list* expiry_list,dict* set_dict);
client_info* client_list_delete(client_list* l,client_info* node);
void handle_set(int connfd,dict *kv_dict,skip_list* expiry_list,char* string);
void handle_get(int connfd,dict *kv_dict,char* string);
void handle_getbit(int connfd,dict *kv_dict,char* string);
void handle_setbit(int connfd,dict *kv_dict,char* string);
void handle_zadd(int connfd,dict *set_dict,char* string);
void handle_zcard(int connfd,dict *set_dict,char* string);

#endif
