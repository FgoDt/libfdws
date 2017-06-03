from ctypes import *


test = cdll.LoadLibrary('../build/libfdws/libfdws_shared.so')


class fdws(Structure):
    _fields_ = [
        ('sockfd', c_int),
        ('wsver', c_int),
        ('port', c_int),
        ('host', c_char_p),
        ('url', c_char_p),
        ('path', c_char_p),
        ('key', c_char_p),
        ('origin', c_char_p),
    ]
pass


class fdmsg(Structure):
    _fields_ = [
        ('FIN', c_int),
        ('opcode', c_int),
        ('hasmask', c_int),
        ('mask', c_byte*4),
        ('payload', c_char_p),
        ('payloadLen', c_int64),
    ]
pass


test.fd_ws_init.restype = POINTER(fdws)
test.fd_msg_alloc.restype = POINTER(fdmsg)

fdwsctx = test.fd_ws_init()
test.fd_ws_connect("http;//localhost/001?name=123", fdwsctx)

msg = test.fd_msg_alloc()
while True:
    test.fd_ws_read_frame(fdwsctx, msg)
    print(msg.contents.payload)
