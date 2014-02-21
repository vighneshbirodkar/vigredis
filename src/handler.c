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
#include "skip_list.h"
 

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
        l->header = l->header->next;
        
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
void client_handle(client_info* client,dict* kv_dict,skip_list* expiry_list)
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
        handle_set(client->fd,kv_dict,expiry_list,buffer_copy);
        return;
    }
    
    if(strcmp(command,"get") == 0)
    {
        handle_get(client->fd,kv_dict,buffer_copy);
        return;
    }
    
    if(strcmp(command,"getbit") == 0)
    {
        handle_getbit(client->fd,kv_dict,buffer_copy);
        return;
    }
    if(strcmp(command,"setbit") == 0)
    {
        handle_setbit(client->fd,kv_dict,buffer_copy);
        return;
    }
    
    //Command is not known
    ret_val = sprintf(reply,VR_REPLY_UNKNOWN_COMMAND,command);

    ret_val = write(client->fd, reply, ret_val);
    if(ret_val < 0)
    {
        perror("write error");
    }
    
}


/*
 * Parses and handles set command
 * will parse string, and reply on connfd
 */
void handle_set(int connfd,dict *kv_dict,skip_list* expiry_list,char* string)
{
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char* command,*key,*value,*next;
    int flag_int = VR_FLAG_NONE;
    int px=-1,ex=-1,i;
    double expiry = -1;
    
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
        ret_val = sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    
    //Parse Value
    value = strtok(NULL," ");
    if(value == NULL)
    {
        //syntax error if value isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, ret_val );
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
                ret_val =sprintf(reply,VR_REPLY_SYNTAX_ERROR);
                ret_val = write(connfd, reply, ret_val );
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
                ret_val =sprintf(reply,VR_REPLY_SYNTAX_ERROR);
                ret_val = write(connfd, reply,ret_val );
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
            ret_val = sprintf(reply,VR_REPLY_SYNTAX_ERROR);
            ret_val = write(connfd, reply, ret_val );
            return;
        }
    }
    next = strtok(NULL," ");
    // String contains extra tokens
    if(next != NULL)
    {
        ret_val =sprintf(reply,VR_REPLY_SYNTAX_ERROR);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    expiry = get_time_ms();
    if(ex > 0)
        expiry += ex;
    if(px > 0)
         expiry += ((double)px)/1000 ;
         
    if( (ex < 0)  && (px < 0))
        expiry = -1;

    //Add if key exists,
    //only VR_ERR_EXIST is the right return value 
    if(expiry > 0)
    {
        //printf("expiry = %lf\n",expiry);
        skip_list_insert(expiry_list,expiry,key,strlen(key));
    }
    
    if(flag_int == VR_FLAG_XX)
    {

        ret_val = dict_add_string(kv_dict,key,strlen(key),value,
                        strlen(value),VR_FLAG_XX,expiry);
                        
        if(ret_val == VR_ERR_EXIST)
        {
            ret_val = sprintf(reply,VR_REPLY_OK);
            ret_val = write(connfd, reply, ret_val );
            return;
        }
        else // Key did not exist
        {
            sprintf(reply,VR_REPLY_NOT_OK);
            ret_val = write(connfd, reply, strlen(reply));
            return;
        }
    }
    
    //Add if key does not exist
    //only VR_ERR_OK is valid return values
    if(flag_int == VR_FLAG_NX)
    {
        ret_val = dict_add_string(kv_dict,key,strlen(key),value,strlen(value),
                VR_FLAG_NX,expiry);
        
        if(ret_val == VR_ERR_OK)
        {
            ret_val = sprintf(reply,VR_REPLY_OK);
            ret_val = write(connfd, reply, ret_val );
            return;
        }
        else
        {
            ret_val = sprintf(reply,VR_REPLY_NOT_OK);
            ret_val = write(connfd, reply, ret_val );
            return;
        }
    }

    //Add key irrespective
    //VR_ERR_OK and VR_ERR_EXIST are valid return values
    if(flag_int == VR_FLAG_NONE)
    {
        ret_val = dict_add_string(kv_dict,key,strlen(key),value,strlen(value),
                VR_FLAG_NONE,expiry);
        
        if((ret_val == VR_ERR_OK) || (ret_val = VR_ERR_EXIST))
        {
            ret_val = sprintf(reply,VR_REPLY_OK);
            ret_val = write(connfd, reply, ret_val );
        }
        else
        {
            ret_val = sprintf(reply,VR_REPLY_NOT_OK);
            ret_val = write(connfd, reply, ret_val );
        }
    }
}


void handle_get(int connfd,dict *kv_dict,char* string)
{
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char* command,*key,*next;
    vr_string *str;
    double expiry,t;
    vr_object *obj;
    
    
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
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    next = strtok(NULL," ");
    //Extra tokens
    if(next != NULL)
    {
        ret_val = sprintf(reply,VR_REPLY_SYNTAX_ERROR);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    if(key == NULL)
    {
        //syntax error if key isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    obj = dict_get(kv_dict,key,strlen(key),&expiry);
    str = &obj->string;
    t = get_time_ms();
    if(str)
    {
        if((expiry > 0) && (expiry < t))
            ret_val = sprintf(reply,VR_REPLY_NOT_FOUND);
        else
            ret_val = sprintf(reply,VR_REPLY_STRING,str->len,str->len,str->string);
        ret_val = write(connfd, reply, ret_val );
    
        if(ret_val < 0)
        {
            perror("Error Writing");
        }
    }
    else
    {
        ret_val =sprintf(reply,VR_REPLY_NOT_FOUND);
        ret_val = write(connfd, reply, ret_val );
    
        if(ret_val < 0)
        {
            perror("Error Writing");
        }
    }
    //printf("L = %d\n",kv_dict->len);
}


void handle_getbit(int connfd,dict *kv_dict,char* string)
{
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char* command,*key,*next;
    char* bit;
    int bit_index;
    int bit_val;
    double expiry,t;
    
    
    rstrip(string);
    command = strtok(string," ");
    str_lower(command);
    
    
    if(strcmp(command,"getbit"))
    {
        perror("Fatal error, command to getbit is not getbit");
    }
    
    //Parse Key
    key = strtok(NULL," ");
    if(key == NULL)
    {
        //syntax error if key isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    bit = strtok(NULL," ");
    if(bit == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    next = strtok(NULL," ");
    //Extra tokens
    if(next != NULL)
    {
        ret_val = sprintf(reply,VR_REPLY_SYNTAX_ERROR);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    if(isint(bit))
        bit_index = atoi(bit);
    else
    {
        ret_val = sprintf(reply,VR_REPLY_WRONG_OFFSET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    bit_val = dict_get_bit(kv_dict,key,strlen(key),bit_index,&expiry);
    t = get_time_ms();
    
    //key expired, as good as key not present
    if((expiry > 0) && (expiry < t))
        bit_val = 0;

    ret_val = sprintf(reply,VR_REPLY_BIT,bit_val);
    ret_val = write(connfd, reply, ret_val );

    if(ret_val < 0)
    {
        perror("Error Writing");
    }
    //printf("L = %d\n",kv_dict->len);
}

void handle_setbit(int connfd,dict *kv_dict,char* string)
{
    
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char* command,*key,*next;
    char* bit;
    char* bit_index_str;
    int bit_index;
    int bit_val;
    double expiry,t;
    
    
    rstrip(string);
    command = strtok(string," ");
    str_lower(command);
    
    if(strcmp(command,"setbit"))
    {
        perror("Fatal error, command to getbit is not getbit");
    }
    
    //Parse Key
    key = strtok(NULL," ");
    if(key == NULL)
    {
        //syntax error if key isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    bit_index_str = strtok(NULL," ");
    if(bit_index_str == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    bit = strtok(NULL," ");
    if(bit == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    
    
    next = strtok(NULL," ");
    //Extra tokens
    if(next != NULL)
    {
        ret_val = sprintf(reply,VR_REPLY_SYNTAX_ERROR);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    if((bit[0] == '1') || (bit[0] == '0'))
        bit_val = atoi(bit);
    else
    {
        ret_val = sprintf(reply,VR_REPLY_WRONG_BIT);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    if(isint(bit_index_str))
        bit_index = atoi(bit_index_str);
    else
    {
        ret_val = sprintf(reply,VR_REPLY_WRONG_OFFSET);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    expiry = -1;
    bit_val = dict_set_bit(kv_dict,key,strlen(key),bit_index,bit_val,&expiry);
    t = get_time_ms();
    
    //key expired, as good as key not present
    if((expiry > 0) && (expiry < t))
        bit_val = 0;

    ret_val = sprintf(reply,VR_REPLY_BIT,bit_val);
    ret_val = write(connfd, reply, ret_val );

    if(ret_val < 0)
    {
        perror("Error Writing");
    }
    //printf("L = %d\n",kv_dict->len);
}



