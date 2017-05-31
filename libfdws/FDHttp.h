//
// Created by li on 17-5-29.
//

#ifndef FDWEBSOCKET_FDHTTP_H
#define FDWEBSOCKET_FDHTTP_H

typedef enum {
    HTTPRES_OK,
    HTTPRES_OK_NOT_MODIFIED,  /* not modified since last request */
    HTTPRES_NOT_FOUND,        /* not found */
    HTTPRES_BAD_REQUEST,      /* client error */
    HTTPRES_SERVER_ERROR,     /* server reported an error */
    HTTPRES_REDIRECTED,       /* resource has been moved */
    HTTPRES_LOST_CONNECTION /* connection lost while waiting for data */
} HTTPResult;

struct HTTP_ctx {
    char *date;
    int size;
    int status;
    void *data;
};

//typedef int (HTTP_read_callback)(void *ptr, int size,int nmemb, void *stream);

//HTTPResult HTTP_get(struct HTTP_ctx *http, const char *url, HTTP_read_callback *cb);

int fd_http_get_with_header(const char *url,char *header);

#endif //FDWEBSOCKET_FDHTTP_H
