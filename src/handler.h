#ifndef __HANDLER_H__
#define __HANDLER_H__

#define VR_ERR_TOK 7
#include "dict.h"

void client_handle(int connfd,dict* kv_dict);
void handle_set(int connfd,dict *kv_dict,char* string);

#endif
