/*
 * A skip list implementation
 * An almost identical implementation of
 * http://www.cl.cam.ac.uk/teaching/2005/Algorithms/skiplists.pdf 
 */
#include "skip_list.h"
#include "vr_string.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include<string.h>
#include "util.h"

/*
 * generates a level 
 * level 2x is half as likely as x
 */
int random_level()
{
    int x = 1;
    while((rand() < (RAND_MAX/2)) && x < VR_SKIP_LIST_MAX_NODE)
        x = x + 1;
    return x;
}

/*
 * Initialization
 */
void skip_list_init(skip_list *sl)
{
    int i;
    sl->level = 1;
    sl->header = (skip_list_node*)malloc(sizeof(skip_list_node));
    sl->header->next =  (skip_list_node**)malloc(VR_SKIP_LIST_MAX_NODE*sizeof(char*));
    for(i=0;i<VR_SKIP_LIST_MAX_NODE;i++)
        sl->header->next[i] = NULL;
}

/*
 * Inserts an element, duplicates allowed
 */
void skip_list_insert(skip_list *sl,double score,char* key,int klen)
{
    int i;
    int v;
    skip_list_node* update[VR_SKIP_LIST_MAX_NODE];
    skip_list_node* x  = sl->header;

    for(i=(sl->level-1); i >= 0 ; i--)
    {
        while( (x->next[i] != NULL) && x->next[i]->score < score )
            x = x->next[i];
        update[i] = x;
    }
    x = x->next[0];
    v = random_level();

    if(v > sl->level)
    {
        for(i=(sl->level );i < v;i++)
            update[i]= sl->header;
        sl->level = v;
    }
    
    x = (skip_list_node*)malloc(sizeof(skip_list_node));
    x->score = score;
    x->key = (char*)malloc(klen);
    strncpy(x->key,key,klen);
    x->klen = klen;
    x->next = (skip_list_node**)malloc(v*sizeof(char*));

    for(i=0;i < v;i++)
    {
        x->next[i] = update[i]->next[i];
        update[i]->next[i] = x;
    }
    
    
}

/*
 *
 */
 
void skip_list_delete_with_key(skip_list *sl,double score,char* key,int klen)
{
    skip_list_node* update[VR_SKIP_LIST_MAX_NODE];
    skip_list_node* x = sl->header;
    int i;
    
    for(i=(sl->level-1); i >= 0 ; i--)
    {
        
        //printf("i = %d\n",i);
        while( ( x->next[i] != NULL) && (x->next[i]->score < score) )
            x = x->next[i];
        update[i] = x;
    }
    //printf("out\n");
    x = x->next[0];
    if( x == NULL)
    {
        printf("Fatal : key not found\n");
        return;
    }
    
    while( (x->next[0] != NULL) && VR_EQ(x->score,score) && (x->klen != klen) && ( strncmp(x->key,key,klen) != 0) )
    {
        x = x->next[0];
    }
    
    if( (x->klen == klen) &&  (strncmp(x->key,key,klen) == 0) )
    {
        for(i=0;i < sl->level;i++)
        {
            if( update[i]->next[i] != x)
                break;
            update[i]->next[i] = x->next[i];
        }
        free(x->key);
        free(x->next);
        free(x);
    
        while( (sl->level > 0) && (sl->header->next[sl->level] == NULL) )
        {
            sl->level--;
        }
    }
    else
        printf("Fatal : key not found\n");
}

/*
 * returns first element
 * minimum value of double if list empty
 */
double skip_list_first(skip_list* sl)
{
    if(sl->header->next[0])
        return sl->header->next[0]->score;
    else
        return DBL_MAX;
}

int skip_list_empty(skip_list* sl)
{
    return (sl->header->next[0] == NULL);
}

skip_list_node* skip_list_pop(skip_list *sl)
{
    int i;
    skip_list_node* first = sl->header->next[0];
    
    //printf("pop\n");
    for(i=0;i<VR_SKIP_LIST_MAX_NODE;i++)
    {
        //printf("i = %d\n",i);
        if(sl->header->next[i] == first )
            sl->header->next[i] = first->next[0];
    }
    return first;

}

/*
 * Debugging purpose
 */
void skip_list_print(skip_list* sl)
{
    skip_list_node* tmp = sl->header;
    tmp = tmp->next[0];
    int counter = 1;
    int trunc;
    while(tmp != NULL)
    {
        trunc = (int)tmp->score;
        if(trunc == tmp->score)
            printf("%d) %d , %.*s \n",counter,trunc,tmp->klen,tmp->key);
        else
            printf("%d) %lf , %.*s \n",counter,tmp->score,tmp->klen,tmp->key);
        tmp = tmp->next[0];
        counter++;
    }
    printf("\n");
}

void skip_list_clear(skip_list* sl)
{
    skip_list_node* tmp;
    while(!skip_list_empty(sl))
    {
        tmp = skip_list_pop(sl);
        free(tmp->key);
        free(tmp->next);
        free(tmp);
        
    }
    
    free(sl->header->next);
    free(sl->header);
}
