#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    #define closesocket close
#endif

int main() {
    SOCKET listenSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    char buffer[1024];

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Erreur WSAStartup\n");
        return 1;
    }
#endif

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("Erreur creation socket\n");
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Erreur bind\n");
        closesocket(listenSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Erreur listen\n");
        closesocket(listenSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    printf("En attente de connexion sur le port 8080...\n");
    clientSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        printf("Erreur accept\n");
        closesocket(listenSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        printf("Recu: %s\n", buffer);
    }

    char *response = "Bonjour du serveur!";
    send(clientSocket, response, (int)strlen(response), 0);

    closesocket(clientSocket);
    closesocket(listenSocket);

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}