#include "../include/net_utils.h"
#include <string.h>
#ifndef _WIN32
    #include <sys/socket.h>
    #include <arpa/inet.h>
#endif

int sendType(SOCKET sock, MessageType type) {
    uint8_t t = (uint8_t)type;
    return send(sock, (const char*)&t, 1, 0) == 1 ? 0 : -1;
}

int recvType(SOCKET sock, MessageType *type) {
    uint8_t t;
    int n = recv(sock, (char*)&t, 1, 0);
    if (n <= 0) return -1;
    *type = (MessageType)t;
    return 0;
}

int sendString(SOCKET sock, const char *str) {
    uint32_t len = str ? (uint32_t)strlen(str) : 0;
    uint32_t netLen = htonl(len);
    int sent = 0;
    const char *lenData = (const char*)&netLen;
    while (sent < (int)sizeof(netLen)) {
        int n = send(sock, lenData + sent, sizeof(netLen) - sent, 0);
        if (n <= 0) return -1;
        sent += n;
    }
    sent = 0;
    while (str && sent < (int)len) {
        int n = send(sock, str + sent, len - sent, 0);
        if (n <= 0) return -1;
        sent += n;
    }
    return 0;
}

int recvString(SOCKET sock, char *buffer, size_t maxLen) {
    uint32_t netLen;
    int received = 0;
    char *lenData = (char*)&netLen;
    while (received < (int)sizeof(netLen)) {
        int n = recv(sock, lenData + received, sizeof(netLen) - received, 0);
        if (n <= 0) return -1;
        received += n;
    }
    uint32_t len = ntohl(netLen);
    if (len >= maxLen) return -1;

    received = 0;
    while (received < (int)len) {
        int n = recv(sock, buffer + received, len - received, 0);
        if (n <= 0) return -1;
        received += n;
    }
    buffer[len] = '\0';
    return 0;
}