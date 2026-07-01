#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../include/user.h"
#include "../include/message.h"
#include "../include/login.h"
#include "protocol.h"
#include "net_utils.h"

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

// Doivent correspondre à ce qu'utilisent login_logic.h / register_logic.h
#ifndef SALT_LEN
#define SALT_LEN 16
#endif
#ifndef HASH_LEN
#define HASH_LEN 32
#endif

typedef struct {
    SOCKET socket;
    int active;
    int loggedIn;
    char pseudo[32];
} Client;

Client clients[MAX_CLIENTS];
SOCKET listenSocket;
volatile int running = 1;


// ---------------------------------------------------------------------
// Envoie la struct User entiere a un client (inchange)
// ---------------------------------------------------------------------
int sendUser(SOCKET sock, User user) {
    uint32_t netId = htonl((uint32_t)user.id);

    int sent = 0;
    const char *idData = (const char*)&netId;
    while (sent < (int)sizeof(netId)) {
        int n = send(sock, idData + sent, sizeof(netId) - sent, 0);
        if (n <= 0) return -1;
        sent += n;
    }

    if (sendString(sock, user.name)      < 0) return -1;
    if (sendString(sock, user.firstName) < 0) return -1;
    if (sendString(sock, user.pseudo)    < 0) return -1;
    if (sendString(sock, user.password)  < 0) return -1;
    if (sendString(sock, user.email)     < 0) return -1;
    if (sendString(sock, user.ip)        < 0) return -1;
    if (sendString(sock, user.rank)      < 0) return -1;
    if (sendString(sock, user.status)    < 0) return -1;

    return 0;
}

// ---------------------------------------------------------------------
// Envoie un message a tous les clients actifs et loggues
// ---------------------------------------------------------------------
void broadcastMessage(const char *message, SOCKET excludeSocket)
{
    LOCK();
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].loggedIn && clients[i].socket != excludeSocket) {

            if (excludeSocket != INVALID_SOCKET && strcmp(clients[i].pseudo, "Client3") == 0) {
                continue;
            }

            send(clients[i].socket, message, (int)strlen(message), 0);
        }
    }
    UNLOCK();
}

// ---------------------------------------------------------------------
// Phase d'authentification : traite salt-request / login / register
// jusqu'a connexion reussie (ou deconnexion du client)
// Retourne 0 si le client est authentifie et pret pour le chat, -1 sinon
// ---------------------------------------------------------------------
static int handleAuthPhase(int index) {
    SOCKET sock = clients[index].socket;
    MessageType type;

    while (running) {
        if (recvType(sock, &type) < 0) return -1;

        if (type == MSG_LOGIN_SALT_REQUEST) {
            char pseudo[64];
            if (recvString(sock, pseudo, sizeof(pseudo)) < 0) return -1;

            char stored[256] = "";
            char salt_hex[SALT_LEN * 2 + 1] = "";
            if (getStoredPassword(pseudo, stored, sizeof(stored)) == 0 &&
                strlen(stored) >= (size_t)(SALT_LEN * 2)) {
                memcpy(salt_hex, stored, SALT_LEN * 2);
                salt_hex[SALT_LEN * 2] = '\0';
            }
            // salt_hex reste vide si le pseudo n'existe pas

            sendType(sock, MSG_LOGIN_SALT_RESPONSE);
            sendString(sock, salt_hex);
            continue;
        }

        if (type == MSG_LOGIN_REQUEST) {
            char pseudo[64], hash[HASH_LEN * 2 + 1];
            if (recvString(sock, pseudo, sizeof(pseudo)) < 0) return -1;
            if (recvString(sock, hash, sizeof(hash)) < 0) return -1;

            User *u = login(pseudo, hash, "127.0.0.1");

            sendType(sock, MSG_LOGIN_RESPONSE);
            if (u) {
                sendString(sock, "OK");
                LOCK();
                strncpy(clients[index].pseudo, pseudo, sizeof(clients[index].pseudo) - 1);
                clients[index].pseudo[sizeof(clients[index].pseudo) - 1] = '\0';
                clients[index].loggedIn = 1;
                UNLOCK();
                freeUser(u);
                return 0; // authentifie, on passe au chat
            } else {
                sendString(sock, "Pseudo ou mot de passe incorrect.");
            }
            continue;
        }

        if (type == MSG_REGISTER_REQUEST) {
            char nom[64], prenom[64], pseudo[64], email[128];
            char salt_hex[SALT_LEN * 2 + 1], hash_hex[HASH_LEN * 2 + 1];

            if (recvString(sock, nom, sizeof(nom)) < 0) return -1;
            if (recvString(sock, prenom, sizeof(prenom)) < 0) return -1;
            if (recvString(sock, pseudo, sizeof(pseudo)) < 0) return -1;
            if (recvString(sock, email, sizeof(email)) < 0) return -1;
            if (recvString(sock, salt_hex, sizeof(salt_hex)) < 0) return -1;
            if (recvString(sock, hash_hex, sizeof(hash_hex)) < 0) return -1;

            // Mot de passe securise stocke = salt_hex + hash_hex concatenes
            char securedPassword[SALT_LEN * 2 + HASH_LEN * 2 + 1];
            snprintf(securedPassword, sizeof(securedPassword), "%s%s", salt_hex, hash_hex);

            int ok = registerUser(nom[0], prenom, pseudo, email, securedPassword);

            sendType(sock, MSG_REGISTER_RESPONSE);
            sendString(sock, ok == 0 ? "OK" : "Erreur lors de l'inscription (pseudo/email deja pris ?).");
            continue;
        }

        // Type inconnu en phase d'authentification : on ignore et on continue
    }

    return -1;
}

// ---------------------------------------------------------------------
// Thread dedie a un client : authentification puis diffusion des messages
// ---------------------------------------------------------------------
THREAD_RET handleClient(void *arg) {
    int index = *(int*)arg;
    free(arg);
    SOCKET sock = clients[index].socket;

    if (handleAuthPhase(index) < 0) {
        closesocket(sock);
        LOCK();
        clients[index].active = 0;
        clients[index].loggedIn = 0;
        UNLOCK();
        printf("\nClient deconnecte avant authentification.\nVous: ");
        fflush(stdout);
#ifdef _WIN32
        return 0;
#else
        return NULL;
#endif
    }

    printf("\n%s authentifie.\nVous: ", clients[index].pseudo);
    fflush(stdout);

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
    clients[index].loggedIn = 0;
    UNLOCK();
    printf("\n%s deconnecte.\nVous: ", clients[index].pseudo);
    fflush(stdout);

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

// ---------------------------------------------------------------------
// Thread d'acceptation (inchange, sauf init loggedIn)
// ---------------------------------------------------------------------
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
        clients[slot].loggedIn = 0;
        snprintf(clients[slot].pseudo, sizeof(clients[slot].pseudo), "Client%d", slot + 1);
        UNLOCK();

        printf("\nNouvelle connexion (slot %d), en attente d'authentification.\nVous: ", slot + 1);
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

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].active = 0;
        clients[i].loggedIn = 0;
    }

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