/*
 * Will be responsible for all communications with a client
 */
#include<strings.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include "handler.h"
#include "dict.h"
#include "util.h"
#include "string_def.h"
 

/*
 * Initialize
 */
void client_list_init(client_list* l)
{
    l->header = NULL;
}

void client_list_add(client_list* l,int fd)
{
    client_info* new = (client_info*)malloc(sizeof(client_info));
    new->fd = fd;
    new->index = 0;
    new->buffer = (char*)malloc(VR_BUFFER_LEN);
    bzero(new->buffer,VR_BUFFER_LEN);
    
    new->next = l->header;
    new->prev = NULL;
    if(l->header)
        l->header->prev = new;
    l->header = new;
}

/*
 * returns pointer to next node
 */
client_info* client_list_delete(client_list* l,client_info* node)
{
    client_info* tmp;
    tmp = node->next;
    if(node == l->header)
    {
        l->header = l->header->next;
        printf("del head\n");
        
    }
    if(node->prev!=NULL)
        node->prev->next = node->next;
    
    if(node->next!=NULL)
        node->next->prev = node->prev;
        
    free(node->buffer);
    free(node);
    return tmp;
}

/*
 * Handles data on connfd
 * Will read and parse the string, and reply appropriately
 */
void client_handle(client_info* client,dict* kv_dict)
{
    char *buffer;
    char buffer_copy[VR_MAX_MSG_LEN];
    char *command;
    char reply[VR_MAX_MSG_LEN];
    int ret_val;

    //bzero( buffer, MAXLINE);       
    //ret_val = read(connfd,buffer,MAXLINE);
    buffer = client->buffer;
    
    
    strcpy(buffer_copy,buffer);
    
    rstrip(buffer);
    command = strtok(buffer," ");
    
    str_lower(command);
    
    if(command == NULL)
    {
        //TODO handle this
        //No reply
        return;
    }
    if(strcmp(command,"set") == 0)
    {
        handle_set(client->fd,kv_dict,buffer_copy);
        return;
    }
    
    if(strcmp(command,"get") == 0)
    {
        handle_get(client->fd,kv_dict,buffer_copy);
        return;
    }
    
    //Command is not known
    sprintf(reply,VR_REPLY_UNKNOWN_COMMAND,command);

    ret_val = write(client->fd, reply, strlen(reply)+1);
    if(ret_val < 0)
    {
        perror("write error");
    }
    
}


/*
 * Parses and handles set command
 * will parse string, and reply on connfd
 */
void handle_set(int connfd,dict *kv_dict,char* string)
{
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char* command,*key,*value,*next;
    int flag_int = VR_FLAG_NONE;
    int px,ex,i;
    
    rstrip(string);
    command = strtok(string," ");
    str_lower(command);
    
    
    if(strcmp(command,"set"))
    {
        perror("Fatal error, command to set is not set");
    }
    
    //Parse Key
    key = strtok(NULL," ");
    if(key == NULL)
    {
        //syntax error if key isn't there
        sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, strlen(reply)+1);
        return;
    }
    
    
    //Parse Value
    value = strtok(NULL," ");
    if(value == NULL)
    {
        //syntax error if value isn't there
        sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, strlen(reply)+1);
        return;
    }
    
    //flag,px,ex
    //3 iterations to see if any are set
    for(i=0;i<3;i++)
    {
        
        next = strtok(NULL," ");
        str_lower(next);
        
        //Nothing more is left, break
        if (next == NULL)
            break;
        
        //px is set, scan next token for int
        else if(strcmp(next,"px") == 0)
        {
            next = strtok(NULL," ");
            if ( isint(next) )
                px = atoi(next);
            else
            {
                sprintf(reply,VR_REPLY_SYNTAX_ERROR);
                ret_val = write(connfd, reply, strlen(reply)+1);
                return;
            }
        }
        //ex is set, scan next token for int
        else if(strcmp(next,"ex") == 0)
        {
            next = strtok(NULL," ");
            if ( isint(next) )
                ex = atoi(next);
            else
            {
                sprintf(reply,VR_REPLY_SYNTAX_ERROR);
                ret_val = write(connfd, reply, strlen(reply)+1);
                return;
            }
        }
        else if(strcmp(next,"xx") == 0)
            flag_int = VR_FLAG_XX;
        else if(strcmp(next,"nx") == 0)
            flag_int = VR_FLAG_NX;
        else
        {
            //some flag which is not known
            sprintf(reply,VR_REPLY_SYNTAX_ERROR);
            ret_val = write(connfd, reply, strlen(reply)+1);
            return;
        }
    }
    next = strtok(NULL," ");
    // String contains extra tokens
    if(next != NULL)
    {
        sprintf(reply,VR_REPLY_SYNTAX_ERROR);
        ret_val = write(connfd, reply, strlen(reply)+1);
        return;
    }
    
    //Add if key exists,
    //only VR_ERR_EXIST is the right return value 
    if(flag_int == VR_FLAG_XX)
    {

        ret_val = dict_add_string(kv_dict,key,strlen(key),value,
                        strlen(value),VR_FLAG_XX);
                        
        if(ret_val == VR_ERR_EXIST)
        {
            sprintf(reply,VR_REPLY_OK);
            ret_val = write(connfd, reply, strlen(reply)+1);
            return;
        }
        else // Key did not exist
        {
            sprintf(reply,VR_REPLY_NOT_OK);
            ret_val = write(connfd, reply, strlen(reply)+1);
            return;
        }
    }
    
    //Add if key does not exist
    //only VR_ERR_OK is valid return values
    if(flag_int == VR_FLAG_NX)
    {
        ret_val = dict_add_string(kv_dict,key,strlen(key),value,strlen(value),VR_FLAG_NX);
        
        if(ret_val == VR_ERR_OK)
        {
            sprintf(reply,VR_REPLY_OK);
            ret_val = write(connfd, reply, strlen(reply)+1);
            return;
        }
        else
        {
            sprintf(reply,VR_REPLY_NOT_OK);
            ret_val = write(connfd, reply, strlen(reply)+1);
            return;
        }
    }

    //Add key irrespective
    //VR_ERR_OK and VR_ERR_EXIST are valid return values
    if(flag_int == VR_FLAG_NONE)
    {
        ret_val = dict_add_string(kv_dict,key,strlen(key),value,strlen(value),VR_FLAG_NONE);
        
        if((ret_val == VR_ERR_OK) || (ret_val = VR_ERR_EXIST))
        {
            sprintf(reply,VR_REPLY_OK);
            ret_val = write(connfd, reply, strlen(reply)+1);
        }
        else
        {
            sprintf(reply,VR_REPLY_NOT_OK);
            ret_val = write(connfd, reply, strlen(reply)+1);
        }
    }
}


void handle_get(int connfd,dict *kv_dict,char* string)
{
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char* command,*key,*next;
    vr_string *str;
    
    rstrip(string);
    command = strtok(string," ");
    str_lower(command);
    
    
    if(strcmp(command,"get"))
    {
        perror("Fatal error, command to get is not get");
    }
    
    //Parse Key
    key = strtok(NULL," ");
    if(key == NULL)
    {
        //syntax error if key isn't there
        sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, strlen(reply)+1);
        return;
    }
    
    next = strtok(NULL," ");
    //Extra tokens
    if(next != NULL)
    {
        sprintf(reply,VR_REPLY_SYNTAX_ERROR);
        ret_val = write(connfd, reply, strlen(reply)+1);
        return;
    }
    
    if(key == NULL)
    {
        //syntax error if key isn't there
        sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, strlen(reply)+1);
        return;
    }
    
    str = dict_get_string(kv_dict,key,strlen(key));
    if(str)
    {
        sprintf(reply,VR_REPLY_STRING,str->len,str->len,str->string);
        ret_val = write(connfd, reply, strlen(reply)+1);
    
        if(ret_val < 0)
        {
            perror("Error Writing");
        }
    }
    else
    {
        sprintf(reply,VR_REPLY_NOT_FOUND);
        ret_val = write(connfd, reply, strlen(reply)+1);
    
        if(ret_val < 0)
        {
            perror("Error Writing");
        }
    }
    //printf("L = %d\n",kv_dict->len);
}


