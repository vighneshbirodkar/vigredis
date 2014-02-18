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
 
#define MAXLINE 1000


void handle_set(int connfd,dict *kv_dict,char* string)
{
    int ret_val;
    char reply[MAXLINE];
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
        sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, strlen(reply)+1);
        return;
    }
    //Parse Value
    value = strtok(NULL," ");
    if(value == NULL)
    {
        sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, strlen(reply)+1);
        return;
    }
    
    //flag,px,ex
    //3 iterations to see if any are set
    for(i=0;i<3;i++)
    {
        
        next = strtok(NULL," ");
        printf("i = %d,next = %s\n",i,next);
        str_lower(next);
        
        //Nothing more is left, break
        if (next == NULL)
        {
            printf("Next NULL\n");
            break;
        }
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
        {
            flag_int = VR_FLAG_XX;
        }
        else if(strcmp(next,"nx") == 0)
        {
            flag_int = VR_FLAG_NX;
        }
        else
        {
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
void client_handle(int connfd,dict* kv_dict)
{
    char buffer[MAXLINE];
    char buffer_copy[MAXLINE];
    char *command;
    char reply[MAXLINE];
    int ret_val;

    bzero( buffer, MAXLINE);       
    ret_val = read(connfd,buffer,MAXLINE);
    
    if(ret_val < 0)
    {
        perror("Error Reading");
    }
    
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
        handle_set(connfd,kv_dict,buffer_copy);
        return;
    }
    
    //Command is not known
    sprintf(reply,VR_REPLY_UNKNOWN_COMMAND,command);

    ret_val = write(connfd, reply, strlen(reply)+1);
    if(ret_val < 0)
    {
        perror("write error");
    }
    
}