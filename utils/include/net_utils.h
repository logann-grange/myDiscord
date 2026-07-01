#ifndef NET_UTILS_H
#define NET_UTILS_H
#include <stdint.h>
#include "protocol.h"

#ifdef _WIN32
    #include <winsock2.h>
    typedef int socklen_t;
#else
    #define SOCKET int
#endif

int sendType(SOCKET sock, MessageType type);
int recvType(SOCKET sock, MessageType *type);
int sendString(SOCKET sock, const char *str);
int recvString(SOCKET sock, char *buffer, size_t maxLen);

#endif