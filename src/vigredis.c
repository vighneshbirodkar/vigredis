#include <netinet/in.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include<strings.h>
#include<string.h>
#include<unistd.h>
#include "vigredis.h"
#include "handler.h"
#include<errno.h>


#define SIZE 1000

int main(int argc,char** argv)
{
    struct timeval select_wait;
    select_wait.tv_sec = VR_SELECT_SEC;
    select_wait.tv_usec = VR_SELECT_USEC;
    struct timeval wait_copy;

    int listenfd, connfd;
    int yes = 1;
    int maxfd = 0;
    int ret_select;
    fd_set read_fds,read_fds_copy;
    struct sockaddr_in servaddr;
    int i;
    
    FD_ZERO(&read_fds_copy);
    FD_ZERO(&read_fds);
    
    listenfd=socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    //fcntl(listenfd, F_SETFL, O_NONBLOCK);

    bzero( &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(VR_PORT);


    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(listenfd, VR_LISTEN_LIMIT);
    
    FD_SET(listenfd,&read_fds);
    maxfd = listenfd;
    
    while(1)
    {
        wait_copy = select_wait;
        read_fds_copy = read_fds;
        ret_select = select(maxfd + 1,&read_fds_copy,NULL,NULL,&wait_copy);

        //exit(0);
        if( ret_select > 0)
        {
            /*
             * Some one has connected
             */
            if (FD_ISSET(listenfd,&read_fds_copy))
            {
                connfd = accept(listenfd,  NULL, NULL);
                FD_SET(connfd,&read_fds);
                maxfd = MAX(maxfd,connfd);
                printf("Client Connected\n");
                
            } // Some data, need to handle it
            else 
            {
                for(i=0;i<maxfd+1;i++)
                {
                    if(FD_ISSET(i,&read_fds_copy))
                    {
                        client_handle(i);
                    }
                }
            }
        }

        
        //if( connections == 0)
        //    usleep(VR_LISTEN_WAIT_TIME);
        
        //client_handle(connfd);
    }
    return 0;

}
