#ifndef __FD_WEB_SOCKET_H__
#define __FD_WEB_SOCKET_H__
#define MAXHEADERLEN 14
typedef struct fdws{
    int         sockfd;
    int         wsver;
    int         port;
    char        *host;
    char        *url;
    char        *path;
    char        *key;
    char        *origin;
}FDWSCtx;

typedef struct fdwsmsg{
    int             FIN;
    int             opcode;
    int             hasmask;
    unsigned char   mask[4];
    unsigned char*  payload;
    unsigned long long   payloadLen;
}FDWSMSG;

char* fd_http_get(FDWSCtx *ctx);

int fd_ws_connect_with_header(const char *url,const char *header,FDWSCtx *ctx);

int fd_ws_connect( char *url,FDWSCtx *ctx);

FDWSCtx *fd_ws_init();

int fd_ws_release(FDWSCtx *ctx);


//0 for ok -1 for error
int fd_parse_url( char *url, FDWSCtx *ctx);

int fd_ws_read_frame(FDWSCtx *ctx, FDWSMSG *msg);
int fd_ws_send_frame(FDWSCtx *ctx, FDWSMSG *msg);


FDWSMSG* fd_msg_alloc();
void fd_msg_release(FDWSMSG *msg);
void fd_encode_uint64(unsigned long long len, unsigned char *data);
void fd_encode_uint16(int len, unsigned char *data);
int fd_get_int16(unsigned char *data);
unsigned long long fd_get_int64(unsigned char *data);

#endif