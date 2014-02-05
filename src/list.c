/*
 * A good ol linked list implementations, for key and values
 */
#include "list.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

void list_init(list *l)
{
    l->len = 0;
    l->root = NULL;
}

int list_add_string(list* l,char* key,int klen,char*value,int vlen,int flag)
{
    list_node* new;
    list_node* tmp;
    
    tmp = list_find(l,key,klen);
    
    if( (flag == VR_FLAG_XX) && (tmp == NULL))
    {
        //printf("ERR NOT EXIST\n");
        return VR_ERR_NOTEXIST;
        
    }
    
    if( (flag == VR_FLAG_NX) && (tmp != NULL))
    {
        //printf("ERR EXIST\n");
        return VR_ERR_EXIST;
    }
    
    if( tmp == NULL)
    {
        new = (list_node*)malloc(sizeof(list_node));
        new->klen = klen;
        new->vlen = vlen;
        new->key = (char*)malloc(klen);
        new->value = (char*)malloc(vlen);
        strncpy(new->key,key,klen);
        strncpy(new->value,value,vlen);
        new->next = l->root;
        l->root = new;
        l->len ++;
        return VR_ERR_OK;
    }
    else
    {
        tmp->vlen = vlen;
        free(tmp->value);
        tmp->value = (char*)malloc(vlen);
        strncpy(tmp->value,value,vlen);
        l->len ++;
        return VR_ERR_OK;
    }
    

}

list_node* list_find(list *l,char* key,int klen)
{
    list_node* tmp = l->root;
    
    while(tmp != NULL)
    {
        if((tmp->klen == klen) && (strncmp(tmp->key,key,klen)==0))
            return tmp;
        tmp = tmp->next;
    }
    return tmp;
}

int list_delete(list *l,char* key,int klen)
{
    list_node* tmp = l->root;
    list_node* prev = NULL;
    
    while(tmp != NULL)
    {
        if((tmp->klen == klen) && (strncmp(tmp->key,key,klen)==0))
            break;
        prev = tmp;
        tmp = tmp->next;
    }
    
    if(tmp == NULL)
    {
        return VR_ERR_NOTEXIST;
    }
    free(tmp->value);
    free(tmp->key);
    
    if(prev == NULL)
        l->root = tmp->next;
    else
        prev->next = tmp->next;
        
    free(tmp);
    l->len--;
    return VR_ERR_OK;
}



void list_print(list *l)
{
    list_node* tmp = l->root;
    
    printf("Length = %u\n",l->len);
    while( tmp != NULL)
    {
        printf("%.*s : %.*s\n", (int)tmp->klen, tmp->key , (int)tmp->vlen, tmp->value);
        tmp = tmp->next;
    }
}
