#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include "FDWebSocket.h"
#include "FDSocket.h"

int fd_ws_read(FDWSCtx *ctx, FDWSMSG *msg);
int fd_ws_send(FDWSCtx *ctx, FDWSMSG* msg);

int fd_ws_connect_with_header(const char *url,const char *header,FDWSCtx *ctx)
{
    return -1;
}

int fd_ws_connect(char *url,FDWSCtx *ctx)
{
    int error = fd_parse_url(url,ctx);
    if(error==-1)
    {
        printf("parse url error! check you url\n");
        return -1;
    }
    ctx->origin = "http://studease.cn";
    char* response = fd_http_get(ctx);
    return -1;
}

FDWSCtx* fd_ws_init()
{
    FDWSCtx *ctx = (FDWSCtx*)malloc(sizeof(FDWSCtx));
    ctx->sockfd=-1;
    ctx->origin = NULL;
    ctx->path = NULL;
    ctx->key = NULL;
    ctx->url = NULL;
    ctx->wsver = 0;
    return  ctx;
}

int fd_ws_release(FDWSCtx *ctx)
{
   // free(ctx->key);
    free(ctx->path);
    //free(ctx->url);
    free(ctx->host);
    ctx->sockfd=-1;
    free(ctx);
}

int fd_parse_url( char *url, FDWSCtx *ctx)
{
    ctx->url = url;
    int url_type_len = 4;
    if(url[0]=='w'&&url[1]=='s')
    {
        url_type_len = 2;
    }

    char* p1 = strchr(url+url_type_len,':');
    if(!p1 || strncmp(p1,"://",3))
        return -1;

    char* hosts = p1+3;
    char* ports = strchr(hosts,':');
    char* path = strchr(hosts,'/');
    if(path==NULL)
    {
        path = url+strlen(url);
        ctx->path = (char*)malloc(2);
        ctx->path[1] = '\0';
        ctx->path[0] = '/';
    } else{
        ctx->path = path;
    }
    if(ports!=NULL)
    {
        ports = ports+1;
        int plen = path-ports;
        char* port = (char*)malloc(plen+1);
        port[plen] = '\0';
        strncpy(port,ports,plen);
        int p = atoi(port);
        if(p!=0)
        {
            free(port);
            ctx->port=p;
        } else{
            free(port);
            return -1;
        }
    } else{
        ctx->port = 80;
    }

    if(path!=NULL)
    {
        int hlen = path-hosts;
        if(ports!=NULL)
        {
            hlen = ports-hosts-1;
        }
    ctx->host =(char*)malloc(hlen+1);
    ctx->host[hlen] = '\0';
    strncpy(ctx->host,hosts,hlen);
    }

}


//-1 for error
int chek_ws_sver(FDWSCtx *ctx,char *response)
{
    return -1;
}


int fd_ws_read_frame(FDWSCtx *ctx, FDWSMSG *msg){

    int ret = 0 ,error = 0;
    ret = fd_ws_read(ctx,msg);
    if(ret<0)
    {
        printf("fd ws read frame error ");
    }
    return ret;
}

int fd_ws_send_frame(FDWSCtx *ctx, FDWSMSG *msg ){
    int ret = 0 ,error = 0;
    ret = fd_ws_send(ctx,msg);
    if(ret<0)
    {
        printf("fd ws send frame error ");
    }
    return ret;
}

int fd_ws_read(FDWSCtx *ctx, FDWSMSG *msg){
    unsigned char buf[MAXHEADERLEN] = {0};
    int offset=0;
    int error = 0;
    int ret = fd_socket_read(ctx->sockfd,&buf[offset],1,&error);
    if(ret <0)
    {
        printf("fd ws read frame data error code:%d\n",error);
        return -1;
    }
    unsigned char temp = buf[offset];
    offset++;
    msg->FIN = temp>>7;
    msg->opcode = temp & 0xf;
    ret = fd_socket_read(ctx->sockfd,&buf[offset],1,&error);
    if(ret <0)
    {
        printf("fd ws read frame data error code:%d\n",error);
        return -1;
    }
    temp = buf[offset];
    offset++;
    msg->hasmask = temp>>7;
    msg->payloadLen = (unsigned long long)temp &0x7f;

    if(msg->payloadLen == 126)
    {
       ret =  fd_socket_read(ctx->sockfd,&buf[offset],2,&error);
        if(ret <0)
        {
            printf("fd ws read frame data error code:%d\n",error);
            return -1;
        }
        unsigned char pbuf[2]={0};
        pbuf[0] = buf[offset];
        pbuf[1] = buf[offset+1];
        offset+=2;
        msg->payloadLen = (unsigned long long)fd_get_int16(pbuf);
    } else if(msg->payloadLen == 127)
    {
        ret = fd_socket_read(ctx->sockfd,&buf[offset],4,&error);
        if(ret <0)
        {
            printf("fd ws read frame data error code:%d\n",error);
            return -1;
        }
        unsigned char pbuf[8]={0};
        memcpy(pbuf,&buf[offset],8);
        offset+=8;
        msg->payloadLen = fd_get_int64(pbuf);
    }

    if(msg->hasmask)
    {
        ret = fd_socket_read(ctx->sockfd,&buf[offset],4,&error);
        if(ret <0)
        {
            printf("fd ws read frame data error code:%d\n",error);
            return -1;
        }
    }

    msg->payload = (unsigned char*)malloc(msg->payloadLen);
    if(msg->hasmask)
    {
        for (int i = 0; i < msg->payloadLen; ++i) {
            msg->payload[i] = msg->payload[i]^msg->mask[i%4];
        }
    }
   ret = fd_socket_read(ctx->sockfd,msg->payload,(int)msg->payloadLen,&error);
    if(ret <0)
    {
        printf("fd ws read frame data error code:%d\n",error);
        return -1;
    }

    return 0;
}

int fd_ws_send(FDWSCtx *ctx, FDWSMSG* msg){
    unsigned char hbuf[MAXHEADERLEN] = {0};
    int offset =0;
    hbuf[offset] = (unsigned char)(msg->FIN<<7);
    hbuf[offset] |= msg->opcode;
    offset ++;
    hbuf[offset] = (unsigned char)(msg->hasmask<<7);
    if(msg->payloadLen<126)
    {
        hbuf[offset] |= msg->payloadLen;
        offset++;
    } else if (msg->payloadLen>126&&msg->payloadLen<65536)
    {
        hbuf[offset] |= 126;
        offset++;
        fd_encode_uint16((int)msg->payloadLen,&hbuf[offset]);
        offset+=2;
    } else {
        hbuf[offset] |= 127;
        offset++;
        fd_encode_uint64(msg->payloadLen,&hbuf[offset]);
        offset+=8;
    }

    if(msg->hasmask)
    {
        memcpy(&hbuf[offset],msg->mask,4);
        offset+=4;
    }

    int ret =0;
    int error = 0;

    if(msg->hasmask)
    {
        for (int i = 0; i < msg->payloadLen; ++i) {
            msg->payload[i] = msg->payload[i]^msg->mask[i%4];
        }
    }

   // ret = fd_socket_send(ctx->sockfd,msg->payload,(int)msg->payloadLen,&error);
    //ret = fd_socket_send(ctx->sockfd,hbuf,offset,&error);
    unsigned  char *sbuf = (unsigned char*)malloc(msg->payloadLen+offset);
    memcpy(sbuf,hbuf,offset);
    memcpy(sbuf+offset,msg->payload,msg->payloadLen);
    ret = fd_socket_send(ctx->sockfd,sbuf,msg->payloadLen+offset,&error);
    if(ret <0)
    {
        printf("fd ws send frame data error code:%d\n",error);
        return -1;
    }


    return 0;
}


FDWSMSG* fd_msg_alloc(){
    FDWSMSG *msg = (FDWSMSG*)malloc(sizeof(FDWSMSG));
    msg->FIN=0;
    msg->hasmask=0;
    msg->opcode=0;
    msg->payloadLen=0;
    msg->mask[0]=0;
    msg->mask[1]=0;
    msg->mask[2]=0;
    msg->mask[3]=0;
    return  msg;
}

void fd_msg_release(FDWSMSG* msg){
    free(msg->payload);
    free(msg);
}

int fd_get_int16(unsigned char *data){

    int i = data[0] << 8;
    i += data[1];
    return  i;
}

void fd_encode_uint16(int len, unsigned char *data)
{
    unsigned char buf[2]={0};
    buf[0] = (unsigned char )((len >>8)&0xff);
    buf[1] = (unsigned char)(len & 0xff);
    memcpy(data,buf,2);
}

void fd_encode_uint64(unsigned long long len, unsigned char *data)
{
    unsigned char buf[8] = {0};
    for (int i = 0; i < 8; ++i) {
        buf[i] = (unsigned char)((len>>(8*(7-i)))&0xff);
    }
    memcpy(data,buf,8);
}

unsigned long long fd_get_int64(unsigned char *data){
    u_int64_t i = (data[0] << 56);
    i += data[1] << 48;
    i += data[2] << 40;
    i += data[3] << 32;
    i += data[4] << 24;
    i += data[5] << 16;
    i += data[6] << 8;
    i += data[7];
    return i;
}

