#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libfdws/FDWebSocket.h"

int main()
{
    FDWSCtx *ctx = fd_ws_init();
    char * abc = "{\"cmd\":\"text\",\"data\":\"123abc send by libfdws\",\"type\":\"multi\",\"channel\":{\"id\":\"001\"}}";
    //fd_ws_connect("http://118.190.79.29:123",ctx);
    fd_ws_connect("http://118.190.79.29/001?name=ff1234",ctx);
    FDWSMSG *msg = fd_msg_alloc();
    fd_ws_read_frame(ctx,msg);
    printf("Read %s\n",msg->payload);
   // fd_msg_release(msg);
//    msg = fd_msg_alloc();
//    fd_ws_read_frame(ctx,msg);
    FDWSMSG *sMsg = fd_msg_alloc();


    sMsg->FIN = 1;
    sMsg->opcode =2;
    sMsg->hasmask = 1;
    sMsg->payloadLen = strlen(abc);
    sMsg->payload =(unsigned char*) malloc(sMsg->payloadLen);
    sMsg->mask[0]=0xc2;
    sMsg->mask[1]=0x6f;
    sMsg->mask[2]=0xdb;
    sMsg->mask[3]=0xf1;
    memcpy(sMsg->payload,abc,sMsg->payloadLen);
    fd_ws_send_frame(ctx,sMsg);
    while (1)
    {
        fd_ws_read_frame(ctx,msg);
        printf("Read %s\n",msg->payload);
        fd_ws_send_frame(ctx,sMsg);
        sleep(1);
    }
   // fd_http_get("http://118.190.79.29/001?name=游客40");
	return 0;
}
