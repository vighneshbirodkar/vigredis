/*
 * A good ol linked list implementations, for key and values
 */
#include "list.h"
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include "vr_object.h"

/*
 * Initialization
 */
void list_init(list *l,char type)
{
    l->type = type;
    l->len = 0;
    l->root = NULL;
}


/*
 * Adds a key, value pair in the list.
 * where value is a string
 * 
 * if flag = VR_FLAG_NONE
 *      values of existing keys are replaced
 * if flag = VR_FLAG_NX
 *      values are only added if key exists
 * if flag = VR_FLAG_XX
 *      values are replaced only if keys exist
 *
 * returns 
 * VR_ERR_OK - if key didn't exist
 * VR_ERR_EXIST - if key existed
 * VR_ERR_NOTEXIST - if key did not exist
 */
int list_add_string(list* l,char* key,int klen,char* value,int vlen,int flag,double expiry)
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
    return list_add_object(l,key,klen,obj,flag,expiry);
}

/*
 * Adds a key, value pair in the list.
 * where value is int
 * 
 * if flag = VR_FLAG_NONE
 *      values of existing keys are replaced
 * if flag = VR_FLAG_NX
 *      values are only added if key exists
 * if flag = VR_FLAG_XX
 *      values are replaced only if keys exist
 *
 * returns 
 * VR_ERR_OK - if key didn't exist
 * VR_ERR_EXIST - if key existed
 * VR_ERR_NOTEXIST - if key did not exist and flag = VR_FLAG_XX
 */
int list_add_int(list *l,char *key,int klen,int value,int flag,double expiry)
{
    if(l->type != VR_TYPE_INT)
    {
        printf("Incompatible List Addition\n");
        return VR_ERR_FATAL;
    }
    
    vr_object obj;
    obj.value = value;
    return list_add_object(l,key,klen,obj,flag,expiry);
    
}

/*
 * Deletes the key, see list_delete_object
 */
int list_delete_int(list *l,char* key,int klen)
{
    return list_delete_object(l,key,klen);
}

/*
 * Deletes the key, see list_delete_object
 */
int list_delete_string(list *l,char* key,int klen)
{
    return list_delete_object(l,key,klen);
}

/*
 * Adds a vr_object to the list, with the given key
 * if flag = VR_FLAG_NONE
 *      values of existing keys are replaced
 * if flag = VR_FLAG_NX
 *      values are only added if key exists
 * if flag = VR_FLAG_XX
 *      values are replaced only if keys exist
 *
 * returns 
 * VR_ERR_OK - if key didn't exist
 * VR_ERR_EXIST - if key existed
 * VR_ERR_NOTEXIST - if key did not existist
 */
int list_add_object(list* l,char* key,int klen,vr_object object,int flag,double expiry)
{
    list_node* new;
    list_node* tmp;
    vr_object old_object;
    
    tmp = list_find(l,key,klen);
    
    if( (flag == VR_FLAG_XX) && (tmp == NULL))
        return VR_ERR_NOTEXIST;
    
    if( (flag == VR_FLAG_NX) && (tmp != NULL))
        return VR_ERR_EXIST;
    
    if( tmp == NULL)
    {
        new = (list_node*)malloc(sizeof(list_node));
        new->klen = klen;
        new->key = (char*)malloc(klen);
        strncpy(new->key,key,klen);

        new->object = object;
        new->expiry = expiry;

        new->next = l->root;
        l->root = new;
        l->len ++;
        return VR_ERR_OK;
    }
    else
    {
        old_object = tmp->object;
        tmp->object = object;
        tmp->expiry = expiry;
        if(l->type == VR_TYPE_STRING)
            free(old_object.string.string);
        
        return VR_ERR_EXIST;
    }
    

}

/*
 * Returns a pointer to the node of the list, with given key
 * NULL, if the key did not exist
 */
 
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

/*
 * Deletes object with given key
 * returns 
 * VR_ERR_EXIST - if object existed and was deleted
 * VR_ERR_NOTEXIST - if key wasn't there
 *
 */
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


/*
 * Deletes object with given key, if it has expred
 * time is assumed to be as given
 * returns 
 * VR_ERR_EXIST - if object existed and was deleted
 * VR_ERR_NOTEXIST - if key wasn't there or it hadn't expired
 *
 */
int list_delete_object_ife(list *l,char* key,int klen, double time)
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
    
    if(tmp->expiry < 0)
        return VR_ERR_NOTEXIST;
        
    if(time > tmp->expiry )
        return VR_ERR_NOTEXIST;
        
    //printf("deleted exp %.*s",klen,key);
    
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







/*
 * Prints a list, debugging purpose
 */
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
