#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    #define THREAD_RET DWORD WINAPI
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <pthread.h>
    #define SOCKET int
    #define INVALID_SOCKET (-1)
    #define SOCKET_ERROR (-1)
    #define closesocket close
    #define THREAD_RET void*
#endif

SOCKET clientSocket;
volatile int running = 1;

// Thread dedie a la reception :
THREAD_RET receiveMessages(void *arg) {
    char buffer[1024];
    int bytesReceived;

    while (running) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            if (running) {
                printf("\nClient deconnecte.\n");
            }
            running = 0;
            break;
        }
        buffer[bytesReceived] = '\0';
        printf("\nClient: %s\nVous: ", buffer);
        fflush(stdout);

        if (strcmp(buffer, "/quit") == 0) {
            running = 0;
            break;
        }
    }
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

int main() {
    SOCKET listenSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    char message[1024];

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

    printf("Client connecte. Tapez /quit pour fermer la conversation.\n");

    // Lancement du thread de reception en arriere-plan
#ifdef _WIN32
    HANDLE recvThread = CreateThread(NULL, 0, receiveMessages, NULL, 0, NULL);
    if (recvThread == NULL) {
        printf("Erreur creation thread\n");
        closesocket(clientSocket);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
#else
    pthread_t recvThread;
    if (pthread_create(&recvThread, NULL, receiveMessages, NULL) != 0) {
        printf("Erreur creation thread\n");
        closesocket(clientSocket);
        closesocket(listenSocket);
        return 1;
    }
#endif

    // thread principal s'occupe de l'envoi
    printf("Vous: ");
    fflush(stdout);
    while (running) {
        if (fgets(message, sizeof(message), stdin) == NULL) {
            running = 0;
            break;
        }
        message[strcspn(message, "\n")] = '\0';

        if (send(clientSocket, message, (int)strlen(message), 0) == SOCKET_ERROR) {
            running = 0;
            break;
        }

        if (strcmp(message, "/quit") == 0) {
            running = 0;
            break;
        }

        if (running) {
            printf("Vous: ");
            fflush(stdout);
        }
    }

    closesocket(clientSocket); // debloque le thread de reception s'il attend un recv()

#ifdef _WIN32
    WaitForSingleObject(recvThread, 2000);
    CloseHandle(recvThread);
#else
    pthread_join(recvThread, NULL);
#endif

    closesocket(listenSocket);

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}