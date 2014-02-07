/*
 * A good ol linked list implementations, for key and values
 */
#include "list.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "vr_object.h"

void list_init(list *l,char type)
{
    l->type = type;
    l->len = 0;
    l->root = NULL;
}


int list_add_string(list* l,char* key,int klen,char* value,int vlen,int flag)
{
    if(l->type != VR_TYPE_STRING)
    {
        printf("Incompatible List Addition\n");
        return VR_ERR_FATAL;
    }
    
    
    vr_object obj;
    obj.string.len = vlen;
    obj.string.string = (char*)malloc(sizeof(char)*vlen);
    strncpy(obj.string.string,value,vlen);
    return list_add_object(l,key,klen,obj,flag);
}

int list_add_int(list *l,char *key,int klen,int value,int flag)
{
    if(l->type != VR_TYPE_INT)
    {
        printf("Incompatible List Addition\n");
        return VR_ERR_FATAL;
    }
    
    vr_object obj;
    obj.value = value;
    return list_add_object(l,key,klen,obj,flag);
    
}

int list_delete_int(list *l,char* key,int klen)
{
    return list_delete_object(l,key,klen);
}

int list_delete_string(list *l,char* key,int klen)
{
    return list_delete_object(l,key,klen);
}


int list_add_object(list* l,char* key,int klen,vr_object object,int flag)
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
        new->key = (char*)malloc(klen);
        strncpy(new->key,key,klen);

        new->object = object;

        new->next = l->root;
        l->root = new;
        l->len ++;
        return VR_ERR_OK;
    }
    else
    {
        tmp->object = object;
        return VR_ERR_EXIST;
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

int list_delete_object(list *l,char* key,int klen)
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
        return VR_ERR_NOTEXIST;
    //free(tm);
    
    if(prev == NULL)
        l->root = tmp->next;
    else
        prev->next = tmp->next;
        
    
    if(l->type == VR_TYPE_STRING)
        free(tmp->object.string.string);
    
    free(tmp->key);
    free(tmp);
    
    l->len--;
    return VR_ERR_EXIST;
}



void list_print(list *l)
{
    list_node* tmp = l->root;
    
    
    printf("Length = %u\n",l->len);
    while( tmp != NULL)
    {
        if(l->type == VR_TYPE_STRING )
            printf("%.*s : %.*s\n", (int)tmp->klen, tmp->key , (int)tmp->object.string.len, tmp->object.string.string);
        if(l->type == VR_TYPE_INT)
            printf("%.*s : %d\n", (int)tmp->klen, tmp->key , (int)tmp->object.value);
        tmp = tmp->next;
    }
}
