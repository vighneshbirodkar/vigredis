/*
 * A kip list implementation 
 */
#include "skip_list.h"
#include "vr_string.h"
#include <stdlib.h>
#include <stdio.h>

int random_level()
{
    int x = 1;
    while((rand() < (RAND_MAX/2)) && x < VR_SKIP_LIST_MAX_NODE)
        x = x + 1;
    return x;
}


void skip_list_init(skip_list *sl)
{
    int i;
    sl->level = 1;
    sl->header = (skip_list_node*)malloc(sizeof(skip_list_node));
    sl->header->next =  (skip_list_node**)malloc(VR_SKIP_LIST_MAX_NODE*sizeof(skip_list_node));
    for(i=0;i<VR_SKIP_LIST_MAX_NODE;i++)
        sl->header->next[i] = NULL;
}

void skip_list_insert(skip_list *sl,double key)
{
    int i;
    int v;
    skip_list_node* update[VR_SKIP_LIST_MAX_NODE];
    skip_list_node* x  = sl->header;

    //printf("One\n");
    for(i=(sl->level-1); i >= 0 ; i--)
    {
        while( (x->next[i] != NULL) && x->next[i]->score < key )
            x = x->next[i];
        update[i] = x;
    }
    //printf("Two\n");
    x = x->next[0];
    //printf("Three\n");
    v = random_level();
    if(v > sl->level)
    {
        //printf("Greater\n");
        
        for(i=(sl->level );i < v;i++)
            update[i]= sl->header;
        sl->level = v;

    }
    //printf("Four\n");
    
    x = (skip_list_node*)malloc(sizeof(skip_list_node));
    x->score = key;
    x->next = (skip_list_node**)malloc(v*sizeof(char*));
    //printf("Five\n");
    for(i=0;i < v;i++)
    {
        //printf("%d\n",(int)update[i]->next[i]);
        x->next[i] = update[i]->next[i];
        update[i]->next[i] = x;
    }
    
    
}

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
            printf("%d --> %d\n",counter,trunc);
        else
            printf("%d --> %lf\n",counter,tmp->score);
        tmp = tmp->next[0];
        counter++;
    }
    printf("\n");
}