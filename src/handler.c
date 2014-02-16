/*
 * Will be responsible for all communications with a client
 */
#include<strings.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
#include "handler.h"
 
#define MAXLINE 1000
void client_handle(int connfd)
{
    char buff[MAXLINE];
    int x;

    bzero( buff, MAXLINE);       
    x = read(connfd,buff,MAXLINE);
    printf("Echo : %s",buff);
    x = write(connfd, buff, strlen(buff)+1);
    x = x + 1;
    
}
