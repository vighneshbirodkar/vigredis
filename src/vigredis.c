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
#include <signal.h>
#include <stddef.h>


#define SIZE 1000

int main(int argc,char** argv)
{

    dict kv_dict;
    struct timeval select_wait;
    select_wait.tv_sec = VR_SELECT_SEC;
    select_wait.tv_usec = VR_SELECT_USEC;
    struct timeval wait_copy;

    int listenfd, connfd;
    int maxfd = 0;
    int ret_select;
    fd_set read_fds,read_fds_copy;
    struct sockaddr_in servaddr;
    int i,yes=1;
    sigset_t sig_mask, pending_mask;
    
    
    // Initializations
    dict_init(&kv_dict,VR_TYPE_STRING);
    
    
    FD_ZERO(&read_fds_copy);
    FD_ZERO(&read_fds);
    
    //Blocking Signals, SIGINT is bad !!
    sigemptyset (&sig_mask);
    sigaddset (&sig_mask, SIGINT);
    sigprocmask (SIG_SETMASK, &sig_mask, NULL);
    
    //Getting ready to listen
    listenfd=socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    //Mandatory stuff
    bzero( &servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
    servaddr.sin_port = htons(VR_PORT);

    //Listening 
    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    listen(listenfd, VR_LISTEN_LIMIT);
    
    //For select call
    FD_SET(listenfd,&read_fds);
    maxfd = listenfd;
    
    while(1)
    {
        wait_copy = select_wait;
        read_fds_copy = read_fds;
        ret_select = select(maxfd + 1,&read_fds_copy,NULL,NULL,&wait_copy);

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
                
            } // Data, Data , Data !!
            else 
            {
                for(i=0;i<maxfd+1;i++)
                {
                    if(FD_ISSET(i,&read_fds_copy))
                    {
                        client_handle(i,&kv_dict);
                    }
                }
            }
        }
        sigpending (&pending_mask);
        if (sigismember (&pending_mask, SIGINT)) 
        {
            dict_clear(&kv_dict);
            printf("Clean Up\n");
            exit(0);
        }
    }
    return 0;

}
