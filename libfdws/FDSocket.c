#include "FDSocket.h"
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <memory.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

//return sockfd
int fd_socket_create(const unsigned char *host, int port)
{
    int sockfd,ret;

    if((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0){
	    printf("fd_socket_create error!\n");
	    return -1;
    }

    //addr
    struct sockaddr_in sa;
    memset(&sa,0,sizeof(struct sockaddr_in));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = inet_addr(host);
    if(sa.sin_addr.s_addr == INADDR_NONE)
    {
        struct hostent *hp = gethostbyname(host);
        if(!hp || !hp->h_addr )
        {
            printf("fd_socket can not get host!\n");
            return -1;
        }
        sa.sin_addr = *(struct in_addr *)hp->h_addr;
    }
  //  memset(&sa.sin_zero,0,8);

    if(connect(sockfd,(struct sockaddr *)&sa,sizeof(struct sockaddr))<0)
    {
        printf("fd_scoket connect error!\n");
        return  -1;
    }

    return sockfd;
}


int fd_socket_send(int socketfd, unsigned char* data, int len,int* errorcode){
    int ret =0;
    int totallen = 0;
    while (totallen<len) {
        if ((ret = send(socketfd, data + totallen, len - totallen, 0)) < 0) {
            *errorcode = errno;
            return -1;
        } else {
            totallen += ret;
        }
    }
    *errorcode = 0;
    return totallen;
}

int fd_socket_read(int socketfd, unsigned char *outData, int len,int *errorcode){
    int ret = 0;
    int totallen = 0;
    while (totallen<len)
    {
       ret= recv(socketfd,outData+totallen,len-totallen,0);
       if(ret <0)
       {
           *errorcode = errno;
           return -1;
       }else{
           totallen += ret;
        };
    }
    *errorcode = 0;
    return totallen;
}