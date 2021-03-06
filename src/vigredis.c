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
#include "util.h"
#include "dump.h"



int main(int argc,char** argv)
{
    //dict responsible to hold all key value pairs
    dict kv_dict;
    
    //list holding keys to expire, in increasing order of expity time
    skip_list expiry_list;
    
    //dict responsible for mapping set names to sets
    dict set_dict;
    
    struct timeval select_wait;
    select_wait.tv_sec = VR_SELECT_SEC;
    select_wait.tv_usec = VR_SELECT_USEC;
    struct timeval wait_copy;
    

    int listenfd, connfd;
    int maxfd = 0;
    int ret_select;
    fd_set read_fds,read_fds_copy;
    struct sockaddr_in servaddr;
    int yes=1,ret;
    sigset_t sig_mask, pending_mask;
    client_list clients;
    client_info* current_client;
    char* filename = VR_FILENAME;
    int port = VR_PORT;
    int opt;
    
    
    // Initializations
    dict_init(&kv_dict,VR_TYPE_STRING);
    client_list_init(&clients);
    skip_list_init(&expiry_list);
    dict_init(&set_dict,VR_TYPE_SET);
    
    
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
    
    while ((opt = getopt(argc, argv, "f:p:")) != -1) 
    {
        switch (opt) 
        {
            case 'f':
                filename = optarg;
                break;
            case 'p':
                if(isint(optarg))
                {
                    port = atoi(optarg);
                }
                else
                {
                    printf("Error : Invalid Port Specified\n");
                }
                break;
              
        }
    }
    
    printf(VR_GREETING);
    printf("\n");
    printf(VR_VERSION);
    printf(VR_SOURCE_URL);
    printf(VR_WARNING);
    printf("\n");
    printf(VR_DETAILS,getpid(),port,filename);
    printf("\n");

    
    load_state(filename,&kv_dict,&set_dict);
    
    while(1)
    {
    
        //maintainence
        dict_delete_expired(&kv_dict,&expiry_list);
        
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
                client_list_add(&clients,connfd);
                maxfd = MAX(maxfd,connfd);
                printf("Client Connected\n");
                
            } // Data, Data , Data !!
            else 
            {
                current_client = clients.header;
                while(current_client!=NULL)
                {

                    if(current_client == current_client->next)
                        perror("Fatal Error in Client List\n");
                    if(FD_ISSET(current_client->fd,&read_fds_copy))
                    {
                        
                        //read into buffer till last char is newline
                        ret = read(current_client->fd,
                            &current_client->buffer[current_client->index],
                            VR_BUFFER_LEN);
                        
                        current_client->index += ret;
                        if((current_client->index > 1) && 
                        (current_client->buffer[current_client->index-2] == 
                            VR_END_CHAR1) &&
                        (current_client->buffer[current_client->index-1] == 
                            VR_END_CHAR2) 
                            )
                        {
                            client_handle(current_client,&kv_dict,&expiry_list,&set_dict,filename);
                            current_client->index = 0;
                            bzero(current_client->buffer,VR_BUFFER_LEN);
                        }
                        if(ret == 0)
                        {
                            //CLient leaving
                            FD_CLR(current_client->fd,&read_fds);
                            read_fds_copy = read_fds;
                            current_client = client_list_delete(&clients,
                            current_client);
                            printf("Client Disconnected\n");
                            continue;
                        }
                        //client_handle(i,&kv_dict);
                    }
                    if(current_client != NULL)
                        current_client = current_client->next;
                    
                }

            }
        }
        
        sigpending (&pending_mask);
        if (sigismember (&pending_mask, SIGINT)) 
        {
            current_client = clients.header;
            while(current_client)
                current_client = client_list_delete(&clients,current_client);
                
            save_state(filename,&kv_dict,&set_dict);
            dict_clear(&set_dict);
            skip_list_clear(&expiry_list);
            dict_clear(&kv_dict);
            printf("\n");
            printf(VR_BYE);
            exit(0);
        }
    }
    return 0;

}
