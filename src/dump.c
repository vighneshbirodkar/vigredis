#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include "dict.h"
#include "set.h"
#include "dump.h"
#include "handler.h"
#include "set.h"
#include<string.h>

/*
 * saves state to file
 */
void save_state(char *fname,dict* kv_dict,dict* set_dict)
{
    int i;
    FILE* file = fopen(fname,"w");
    list_node* tmp;
    set* set_ptr;
    skip_list_node* node;
    
    if(file == NULL)
    {
        printf("Error : Can't open %s to dump data\n",fname);
        return;
    }
    if(kv_dict->type != VR_TYPE_STRING)
        perror("save_sate: key-value dict incorrect\n");
    for(i=0; i<kv_dict->size; i++)
    {
        tmp = (&kv_dict->table[i])->root;
        while( tmp != NULL)
        {
            //one space indicates a dict entry
            fprintf(file," %.*s %.*s %lf\n", (int)tmp->klen, tmp->key , (int)tmp->object.string.len, tmp->object.string.string, 
            tmp->expiry);
            tmp = tmp->next;
        }
    }
    
    if(set_dict->type != VR_TYPE_SET)
        perror("save_sate: set dict incorrect\n");
    for(i=0; i<set_dict->size; i++)
    {
        tmp = (&set_dict->table[i])->root;
        while( tmp != NULL)
        {
            set_ptr = tmp->object.obj_set;
            node = set_ptr->set_list.header->next[0];
            
            while(node != NULL)
            {
                //2 spaces indicate a set entry
                fprintf(file,"  %.*s %lf %.*s\n", tmp->klen, tmp->key, node->score, node->klen,node->key);
                node = node->next[0];
            }
            tmp = tmp->next;
        }
    }
    fclose(file);
}

/*
 * loads state from file
 */
void load_state(char *fname,dict* kv_dict,dict* set_dict)
{

    FILE* file = fopen(fname,"r");
    char *line = NULL;
    int read;
    size_t len = VR_BUFFER_LEN;
    char key[VR_BUFFER_LEN];
    char value[VR_BUFFER_LEN];
    char set_name[VR_BUFFER_LEN];
    char member[VR_BUFFER_LEN];
    double score;
    double exp;
    vr_object * obj_ptr;
    set* set_ptr;
    vr_object obj;
    
    
    if(file == NULL)
    {
        printf("Error : Can't open %s to load data\n",fname);
        return;
    }
    if(kv_dict->type != VR_TYPE_STRING)
        perror("load_sate: key-value dict incorrect\n");

    
    while ((read = getline(&line, &len, file)) != -1)
    {
        if(read < 2)
        {
            printf("Error Parsing File\n");
            return;
        }
        if((line[0] == ' ') && (line[1] == ' ') )
        {
            sscanf(line,"%s %lf %s",set_name,&score,member);
            obj_ptr = dict_get(set_dict,set_name,strlen(set_name),&exp);
            printf("%s %lf %s\n",set_name,score,member);
            if( obj_ptr == NULL )
            {
                //needs to be malloced
                //decalring a set variable would make it just local to the function
                set_ptr = (set*)malloc(sizeof(set));
                obj.obj_set = set_ptr;
                set_init(set_ptr);
                
                set_add(set_ptr,member,strlen(member),score);
                dict_add_object(set_dict,set_name,strlen(set_name),obj,VR_FLAG_NONE,-1);
                
                
            }
            else
            {
                set_ptr = obj_ptr->obj_set;
                set_add(set_ptr,member,strlen(member),score);
            }
               
        }   
        else if( line[0] == ' ')
        {
            sscanf(line,"%s %s %lf",key,value,&exp);
            dict_add_string(kv_dict,key,strlen(key),value,strlen(value),VR_FLAG_NONE, exp);
        
        }    
        else
        {
            printf("Error Parsing File\n");
            return; 
        }
    }
    
    if(set_dict->type != VR_TYPE_SET)
        perror("load_sate: set dict incorrect\n");
}
