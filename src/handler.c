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
#include "set.h"
#include "dump.h"
 

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
void client_handle(client_info* client,dict* kv_dict,skip_list* expiry_list,dict* set_dict, char* filename)
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
    
    if(strcmp(command,"zadd") == 0)
    {
        handle_zadd(client->fd,set_dict,buffer_copy);
        return;
    }
    
    if(strcmp(command,"zcard") == 0)
    {
        handle_zcard(client->fd,set_dict,buffer_copy);
        return;
    }
    
    if(strcmp(command,"zrange") == 0)
    {
        handle_zrange(client->fd,set_dict,buffer_copy);
        return;
    }
    if(strcmp(command,"zcount")==0)
    {
        handle_zcount(client->fd,set_dict,buffer_copy);
        return;
    }
    if(strcmp(command,"save")==0)
    {
        printf("Saving state to : %s\n",filename);
        save_state(filename,kv_dict,set_dict);
        ret_val = sprintf(reply,VR_REPLY_OK);
        ret_val = write(client->fd, reply, ret_val);
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
        ret_val = sprintf(reply,VR_REPLY_WRONG_ARG_SET,"set");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    
    //Parse Value
    value = strtok(NULL," ");
    if(value == NULL)
    {
        //syntax error if value isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET, "set");
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
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"get");
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
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"get");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    //check for key, if not present, return VR_REPLY_NOT_FOUND
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
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"getbit");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    bit = strtok(NULL," ");
    if(bit == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"getbit");
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
    
    //check for key, if not return 0
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
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"setbit");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    bit_index_str = strtok(NULL," ");
    if(bit_index_str == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"setbit");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    bit = strtok(NULL," ");
    if(bit == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"setbit");
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
    //set the bit, the call handles the key not present case
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

void handle_zadd(int connfd,dict *set_dict,char* string)
{
    
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char* command,*set_str,*score,*member,*next;
    double score_val,exp;
    vr_object *obj_ptr;
    vr_object obj;
    set* set_ptr;
    int exist;

    
    
    rstrip(string);
    command = strtok(string," ");
    str_lower(command);
    
    if(strcmp(command,"zadd"))
    {
        perror("Fatal error, command to zadd is not zadd");
    }
    
    //Parse Key
    set_str = strtok(NULL," ");
    if(set_str == NULL)
    {
        //syntax error if key isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zadd");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    score = strtok(NULL," ");
    if(score == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zadd");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    member = strtok(NULL," ");
    if(member == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zadd");
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
     
    if(isdouble(score))
        score_val = atof(score);
    else
    {
        ret_val = sprintf(reply,VR_REPLY_NOT_FLOAT);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    //check if set exists, if not, create a new one and add member
    obj_ptr = dict_get(set_dict,set_str,strlen(set_str),&exp);
    if( obj_ptr == NULL )
    {
        //needs to be malloced
        //decalring a set variable would make it just local to the function
        set_ptr = (set*)malloc(sizeof(set));
        obj.obj_set = set_ptr;
        set_init(set_ptr);
        
        set_add(set_ptr,member,strlen(member),score_val);
        dict_add_object(set_dict,set_str,strlen(set_str),obj,VR_FLAG_NONE,-1);
        
        ret_val = sprintf(reply,VR_REPLY_BIT,1);
        ret_val = write(connfd, reply, ret_val );
        
    }
    else
    {
        set_ptr = obj_ptr->obj_set;
        exist = set_add(set_ptr,member,strlen(member),score_val);
        
        ret_val = -1;
        if(exist == VR_ERR_OK)
            ret_val = sprintf(reply,VR_REPLY_BIT,1);
        if(exist == VR_ERR_EXIST)
            ret_val = sprintf(reply,VR_REPLY_BIT,0);
            
        if(ret_val == -1)
        {
            perror("Fatal : Set Operation Error\n");
        }
        else
            ret_val = write(connfd, reply , ret_val);
        
    }
    //dict_print(set_dict);
    //printf("Add %lf , %.*s to set %.*s\n",score_val,(int)strlen(member),member,(int)strlen(set_str),set_str);
}


void handle_zcard(int connfd,dict *set_dict,char* string)
{
    
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char* command,*set_str,*next;
    double exp;
    vr_object *obj_ptr;
    
    rstrip(string);
    command = strtok(string," ");
    str_lower(command);
    
    if(strcmp(command,"zcard"))
    {
        perror("Fatal error, command to zcard is not zcard");
    }
    
    //Parse Key
    set_str = strtok(NULL," ");
    if(set_str == NULL)
    {
        //syntax error if key isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zcard");
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
     

    
    obj_ptr = dict_get(set_dict,set_str,strlen(set_str),&exp);
    if( obj_ptr == NULL )
    {
        ret_val = sprintf(reply,VR_REPLY_BIT,0);
        ret_val = write(connfd, reply, ret_val );
        
    }
    else
    {
        ret_val = sprintf(reply,VR_REPLY_BIT,obj_ptr->obj_set->len);
        ret_val = write(connfd, reply, ret_val );
    }
    //dict_print(set_dict);
    //printf("Add %lf , %.*s to set %.*s\n",score_val,(int)strlen(member),member,(int)strlen(set_str),set_str);
}


void handle_zrange(int connfd,dict *set_dict,char* string)
{
    
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char score_reply[VR_MAX_MSG_LEN];
    char* command,*set_str,*stop,*start,*next;
    double exp;
    int ws = 0;
    vr_object *obj_ptr;
    int start_val,stop_val;
    set* set_ptr;
    int count;
    int offset1,offset2,i;
    skip_list_node* node;
    int len;
    int trunc;
    int str_offset = 0;
    
    rstrip(string);
    command = strtok(string," ");
    str_lower(command);
    
    if(strcmp(command,"zrange"))
    {
        perror("Fatal error, command to zrange is not zrange");
    }
    
    //Parse Key
    set_str = strtok(NULL," ");
    if(set_str == NULL)
    {
        //syntax error if key isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zrange");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    start = strtok(NULL," ");
    if(start == NULL)
    {
        //syntax error if key isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zrange");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    stop = strtok(NULL," ");
    if(stop == NULL)
    {
        //syntax error if key isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zrange");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    next = strtok(NULL," ");
    //Extra tokens
    if(next != NULL)
    {
        str_lower(next);
        if( strcmp (next,"withscores") == 0)
        {
            ws = 1;
        }
        else
        {
            ret_val = sprintf(reply,VR_REPLY_SYNTAX_ERROR);
            ret_val = write(connfd, reply, ret_val );
            return;
        }
    }
    
    if(isint_neg(start))
        start_val = atoi(start);
    else
    {
        ret_val = sprintf(reply,VR_REPLY_NOT_INT);
        ret_val = write(connfd, reply, ret_val );
        return;
    }

    if(isint_neg(stop))
        stop_val = atoi(stop);
    else
    {
        ret_val = sprintf(reply,VR_REPLY_NOT_INT);
        ret_val = write(connfd, reply, ret_val );
        return;
    }   
    
    obj_ptr = dict_get(set_dict,set_str,strlen(set_str),&exp);
    
    //get indices and convert them to the 0 biased offsets
    
    if(obj_ptr == NULL )
    {
        ret_val = sprintf(reply,VR_REPLY_ARRAY_SIZE,0);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    else
    {
        set_ptr = obj_ptr->obj_set;
        offset1 = (start_val < 0) ? (set_ptr->len + start_val) : start_val;
        offset2 = (stop_val < 0) ? (set_ptr->len + stop_val) : stop_val;
    }
    
    
    
    if(offset1 > offset2)
    {
        ret_val = sprintf(reply,VR_REPLY_ARRAY_SIZE,0);
        ret_val = write(connfd, reply, ret_val );
        return;
    }

    if(offset1 >= set_ptr->len)
    {
        ret_val = sprintf(reply,VR_REPLY_ARRAY_SIZE,0);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
        
    if(offset2 >= set_ptr->len)
        offset2 = set_ptr->len - 1;  

    count = offset2 - offset1 + 1;
    if(count < 0)
        count = 0;
        
    if(ws)
        ret_val = sprintf(reply + str_offset,VR_REPLY_ARRAY_SIZE,count*2);
    else
        ret_val = sprintf(reply + str_offset,VR_REPLY_ARRAY_SIZE,count);
    str_offset += ret_val;
    //ret_val = write(connfd, reply, ret_val );
    
    node = set_ptr->set_list.header;
    node = node->next[0];
    
    //printf("of1 = %d , of2 = %d , count = %d\n",offset1,offset2,count);
    
    //loop till required
    for(i=0;i<offset1;i++)
    {
        if(node == NULL)
            perror("Fatal : zrange\n");
        node = node->next[0];
    }
    
    for(i=0;i<count;i++)
    {
        if(node == NULL)
            perror("Fatal : zrange\n");
        //printf(" i = %d\n",i);
        ret_val = sprintf(reply + str_offset,VR_REPLY_STRING,node->klen,node->klen,node->key);
        str_offset += ret_val;
        //ret_val = write(connfd, reply, ret_val );
        
        if(ws)
        {
            trunc = (int)node->score;
            if( trunc == node->score)
                len = sprintf(score_reply,"%d",(int)node->score);
            else
                len = sprintf(score_reply,"%lf",node->score);
                
            ret_val = sprintf(reply + str_offset,VR_REPLY_STRING,len,len,score_reply);
            str_offset += ret_val;
            //ret_val = write(connfd, reply, ret_val );
            
        }
        node = node->next[0];
    }

    ret_val = write(connfd, reply, str_offset );
    //dict_print(set_dict);
    //printf("Add %lf , %.*s to set %.*s\n",score_val,(int)strlen(member),member,(int)strlen(set_str),set_str);
}


void handle_zcount(int connfd,dict *set_dict,char* string)
{
    
    int ret_val;
    char reply[VR_MAX_MSG_LEN];
    char* command,*set_str,*min,*max,*next;
    double max_val,min_val,exp;
    vr_object *obj_ptr;
    skip_list_node *start;
    int count = 0;
    
    
    rstrip(string);
    command = strtok(string," ");
    str_lower(command);
    
    if(strcmp(command,"zcount"))
    {
        perror("Fatal error, command to zcount is not zcount");
    }
    
    //Parse Key
    set_str = strtok(NULL," ");
    if(set_str == NULL)
    {
        //syntax error if key isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zcount");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    min = strtok(NULL," ");
    if(min == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zcount");
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    max = strtok(NULL," ");
    if(max == NULL)
    {
        //syntax error if bit isn't there
        ret_val =sprintf(reply,VR_REPLY_WRONG_ARG_SET,"zcount");
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
     
    if(isdouble(max))
        max_val = atof(max);
    else
    {
        ret_val = sprintf(reply,VR_REPLY_NOT_FLOAT);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    if(isdouble(min))
        min_val = atof(min);
    else
    {
        ret_val = sprintf(reply,VR_REPLY_NOT_FLOAT);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    obj_ptr = dict_get(set_dict,set_str,strlen(set_str),&exp);
    count = 0;
    if(obj_ptr == NULL)
    {
        ret_val = sprintf(reply,VR_REPLY_BIT,count);
        ret_val = write(connfd, reply, ret_val );
        return;
    }
    
    //keep looping and count
    start = skip_list_point(&obj_ptr->obj_set->set_list,min_val);
    
    while( (start != NULL) && (start->score <= max_val))
    {
        count ++;
        start = start->next[0];
    }

    ret_val = sprintf(reply,VR_REPLY_BIT,count);
    ret_val = write(connfd, reply, ret_val );
    return;
    
}

