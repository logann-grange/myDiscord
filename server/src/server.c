#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/channel.h"
#include "../include/user.h"
#include "../include/message.h"


#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
    #define THREAD_RET DWORD WINAPI
    CRITICAL_SECTION clientsMutex;
    #define LOCK() EnterCriticalSection(&clientsMutex)
    #define UNLOCK() LeaveCriticalSection(&clientsMutex)
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
    pthread_mutex_t clientsMutex = PTHREAD_MUTEX_INITIALIZER;
    #define LOCK() pthread_mutex_lock(&clientsMutex)
    #define UNLOCK() pthread_mutex_unlock(&clientsMutex)
#endif

#define MAX_CLIENTS 10
#define PORT 8080

typedef struct {
    SOCKET socket;
    int active;
    char pseudo[32];
} Client;

Client clients[MAX_CLIENTS];
SOCKET listenSocket;
volatile int running = 1;

// Envoie un message a tous les clients actifs, sauf eventuellement l'emetteur
void broadcastMessage(const char *message, SOCKET excludeSocket) {
    LOCK();
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].socket != excludeSocket) {
            send(clients[i].socket, message, (int)strlen(message), 0);
        }
    }
    UNLOCK();
}

// Thread dedie a un client : recoit ses messages et les diffuse aux autres
THREAD_RET handleClient(void *arg) {
    int index = *(int*)arg;
    free(arg);
    SOCKET sock = clients[index].socket;
    char buffer[1024];
    int bytesReceived;

    while (running) {
        bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break;
        buffer[bytesReceived] = '\0';

        char fullMessage[1100];
        snprintf(fullMessage, sizeof(fullMessage), "%s: %s", clients[index].pseudo, buffer);
        printf("\n%s\nVous: ", fullMessage);
        fflush(stdout);

        broadcastMessage(fullMessage, sock);

        if (strcmp(buffer, "/quit") == 0) break;
    }

    closesocket(sock);
    LOCK();
    clients[index].active = 0;
    UNLOCK();
    printf("\n%s deconnecte.\nVous: ", clients[index].pseudo);
    fflush(stdout);

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

// Thread d'acceptation : tourne en boucle pour accepter de nouveaux clients
THREAD_RET acceptClients(void *arg) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    while (running) {
        SOCKET newSocket = accept(listenSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (newSocket == INVALID_SOCKET) {
            if (running) printf("Erreur accept\n");
            continue;
        }

        LOCK();
        int slot = -1;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (!clients[i].active) { slot = i; break; }
        }
        if (slot == -1) {
            UNLOCK();
            printf("\nServeur plein, connexion refusee.\nVous: ");
            fflush(stdout);
            closesocket(newSocket);
            continue;
        }
        clients[slot].socket = newSocket;
        clients[slot].active = 1;
        snprintf(clients[slot].pseudo, sizeof(clients[slot].pseudo), "Client%d", slot + 1);
        UNLOCK();

        printf("\n%s connecte.\nVous: ", clients[slot].pseudo);
        fflush(stdout);

        int *indexArg = malloc(sizeof(int));
        *indexArg = slot;

#ifdef _WIN32
        HANDLE h = CreateThread(NULL, 0, handleClient, indexArg, 0, NULL);
        if (h) CloseHandle(h);
#else
        pthread_t tid;
        pthread_create(&tid, NULL, handleClient, indexArg);
        pthread_detach(tid);
#endif
    }
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}


int main() {
    struct sockaddr_in serverAddr;
    char message[1024];

#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Erreur WSAStartup\n");
        return 1;
    }
    InitializeCriticalSection(&clientsMutex);
#endif

    for (int i = 0; i < MAX_CLIENTS; i++) clients[i].active = 0;

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        printf("Erreur creation socket\n");
        return 1;
    }

    int opt = 1;
#ifdef _WIN32
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Erreur bind\n");
        closesocket(listenSocket);
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Erreur listen\n");
        closesocket(listenSocket);
        return 1;
    }

    printf("Serveur en attente de connexions sur le port %d...\n", PORT);

#ifdef _WIN32
    HANDLE acceptThread = CreateThread(NULL, 0, acceptClients, NULL, 0, NULL);
#else
    pthread_t acceptThread;
    pthread_create(&acceptThread, NULL, acceptClients, NULL);
#endif

    // Le thread principal diffuse les messages de l'admin a tous les clients
    printf("Vous: ");
    fflush(stdout);
    while (running) {
        if (fgets(message, sizeof(message), stdin) == NULL) {
            running = 0;
            break;
        }
        message[strcspn(message, "\n")] = '\0';

        char fullMessage[1100];
        snprintf(fullMessage, sizeof(fullMessage), "Serveur: %s", message);
        broadcastMessage(fullMessage, INVALID_SOCKET);

        if (strcmp(message, "/quit") == 0) {
            running = 0;
            break;
        }
        printf("Vous: ");
        fflush(stdout);
    }

    LOCK();
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active) closesocket(clients[i].socket);
    }
    UNLOCK();

    closesocket(listenSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}