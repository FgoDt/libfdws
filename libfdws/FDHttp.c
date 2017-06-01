#include <memory.h>
#include <stdlib.h>
#include "FDWebSocket.h"
#include "FDHttp.h"
#include "FDSocket.h"
#include <stdio.h>

#define GET "GET "
#define HTTP1 " HTTP/1.1\r\n"
#define RET "\r\n"
#define ORIGIN "Origin: "
#define HOST "Host: "
#define CONNECTION "Connection: "
#define UPGRADE "Upgrade: "
#define WSVER "Sec-WebSocket-Version: 13\r\n"
#define WSPROTOCOL "Sec-WebSocket-Protocol: binary\r\n"
#define WSUSERAGENT "User-Agent: Mozilla/5.0 (libfdws) AppleWebKit/537.36\r\n"
#define WSKEY "Sec-WebSocket-Key: "

char* fd_http_default_header(const char *path,const char *host,const char *wsKey,const char *origin);
char* fd_http_read(FDWSCtx *ctx);


char* fd_http_get(FDWSCtx *ctx)
{

    ctx->sockfd = fd_socket_create((const unsigned char*) (ctx->host),ctx->port);
    if(ctx->sockfd<0)
        return NULL;

    char *headerTemp = fd_http_default_header(ctx->path,ctx->host,"ezMcxP7L/TFzSga1ISrfKz+BDcA=",ctx->origin);
    char *header = (char*)malloc(strlen(headerTemp)+3);
    memset(header,0,strlen(headerTemp+3));
    strcat(header,headerTemp);
    strcat(header,RET);
    free(headerTemp);
    int error = 0;
    int ret = fd_socket_send(ctx->sockfd,header,strlen(header),&error);
    if (ret == -1)
    {
        printf("fd http get error code :%d\n",error);
        return NULL;
    }
    free(header);

    char *response = fd_http_read(ctx);

    printf("%s\n",response);
    return response;
}

int fd_http_get_with_header(const char *url,char *header){
    return -1;
}

char* fd_http_read(FDWSCtx *ctx)
{
    unsigned  char buf[1024]={0};
    int markIndex = 0;
    int len = 0;
    while (markIndex<4)
    {
        int errorcode = 0;
        if(fd_socket_read(ctx->sockfd,&buf[len],1,&errorcode)<0){
            printf("socket errno code:=%d\n",errorcode);
            return NULL;
        }
        if(markIndex==0&&buf[len]=='\r')
        {
            markIndex=1;
            len++;
            continue;
        }
        if(markIndex==1)
        {
            if(buf[len]=='\n') {
                markIndex = 2;
                len++;
                continue;
            } else {
                markIndex = 0;
            }
        }
        if(markIndex==2)
        {
            if(buf[len] == '\r') {
                markIndex = 3;
                len++;
                continue;
            }else {
                markIndex = 0;
            }
        }
        if(markIndex==3) {
            if (buf[len] == '\n') {
                markIndex = 4;
                len++;
                continue;
            } else {
                markIndex = 0;
            }
        }
        len++;
    }
    char *response = malloc(len+1);
    response[len]='\0';
    memcpy(response,&buf,(size_t)len);
    return response;

}

char* fd_http_default_header(const char *path,const char *host,const char *wsKey,const char *origin){
    char result[1024]={0};
    strcat(result,GET);
    strcat(result,path);
    strcat(result,HTTP1);
    strcat(result,ORIGIN);
    strcat(result,origin);
    strcat(result,RET);
    strcat(result,HOST);
    strcat(result,host);
    strcat(result,RET);
    strcat(result,CONNECTION);
    strcat(result,"Upgrade\r\n");
    strcat(result,UPGRADE);
    strcat(result,"websocket\r\n");
    strcat(result,WSVER);
    strcat(result,WSPROTOCOL);
    strcat(result,WSUSERAGENT);
    strcat(result,WSKEY);
    strcat(result,wsKey);
    strcat(result,RET);
    size_t len = strlen(result);
    char * header = (char*)malloc(len);
    memset(header,0,len+1);
    memcpy(header,result,len);
    return header;
}
