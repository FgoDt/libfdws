int fd_socket_create(const unsigned char *host, int port);

int fd_socket_send(int socketfd, unsigned char* data, int len,int *errorcode);

int fd_socket_read(int socketfd, unsigned char *outData,int len,int *errorcode);