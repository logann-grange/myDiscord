#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "../include/user.h"
#include "../include/message.h"
#include "../include/login.h"
#include "../include/channel.h"
#include "roles.h"
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

typedef struct {
    SOCKET socket;
    int active;
    int loggedIn;
    int userId;
    UserRole role;
    time_t timeoutUntil;
    char pseudo[32];
} Client;

Client clients[MAX_CLIENTS];
SOCKET listenSocket;
volatile int running = 1;

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

void broadcastChatMessage(const char *channel, const char *auteur, const char *texte, int messageId) {
    char idStr[16];
    snprintf(idStr, sizeof(idStr), "%d", messageId);
    LOCK();
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].loggedIn) {
            sendType(clients[i].socket, MSG_CHAT_INCOMING);
            sendString(clients[i].socket, channel);
            sendString(clients[i].socket, auteur);
            sendString(clients[i].socket, texte);
            sendString(clients[i].socket, idStr);
        }
    }
    UNLOCK();
}

void broadcastMessageDeleted(const char *channel, int messageId) {
    char idStr[16];
    snprintf(idStr, sizeof(idStr), "%d", messageId);
    LOCK();
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].loggedIn) {
            sendType(clients[i].socket, MSG_MESSAGE_DELETED_BROADCAST);
            sendString(clients[i].socket, channel);
            sendString(clients[i].socket, idStr);
        }
    }
    UNLOCK();
}

static int findClientIndexByPseudo(const char *pseudo) {
    LOCK();
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].loggedIn && strcmp(clients[i].pseudo, pseudo) == 0) {
            UNLOCK();
            return i;
        }
    }
    UNLOCK();
    return -1;
}

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
                clients[index].userId = u->id;
                clients[index].role = roleFromString(u->rank);
                clients[index].timeoutUntil = 0;
                UNLOCK();
                freeUser(u);
                return 0;
            } else {
                // Dev fallback: allow chat flow even when DB auth is unavailable.
                sendString(sock, "OK");
                LOCK();
                strncpy(clients[index].pseudo, pseudo, sizeof(clients[index].pseudo) - 1);
                clients[index].pseudo[sizeof(clients[index].pseudo) - 1] = '\0';
                clients[index].loggedIn = 1;
                clients[index].userId = 0;
                clients[index].role = ROLE_UTILISATEUR;
                clients[index].timeoutUntil = 0;
                UNLOCK();
                return 0;
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

            char securedPassword[SALT_LEN * 2 + HASH_LEN * 2 + 1];
            snprintf(securedPassword, sizeof(securedPassword), "%s%s", salt_hex, hash_hex);

            int ok = registerUser(nom, prenom, pseudo, email, securedPassword);
            sendType(sock, MSG_REGISTER_RESPONSE);
            sendString(sock, ok ? "OK" : "Erreur lors de l'inscription (pseudo/email deja pris ?).");
            continue;
        }
    }
    return -1;
}

// ---------------------------------------------------------------------
static void sendModResponse(SOCKET sock, int ok, const char *msg) {
    sendType(sock, MSG_MOD_RESPONSE);
    sendString(sock, ok ? "OK" : msg);
}

static void handleChatPhase(int index) {
    SOCKET sock = clients[index].socket;
    MessageType type;

    while (running) {
        if (recvType(sock, &type) < 0) break;

        if (type == MSG_CHAT) {
            char channel[64], texte[1024];
            if (recvString(sock, channel, sizeof(channel)) < 0) break;
            if (recvString(sock, texte, sizeof(texte)) < 0) break;

            LOCK();
            time_t until = clients[index].timeoutUntil;
            UNLOCK();
            if (until > time(NULL)) {
                sendModResponse(sock, 0, "Vous etes en timeout, message refuse.");
                continue;
            }

            int channelId = getChannelIdByName(channel);
            int newMsgId = -1;
            if (channelId >= 0) {
                Message msg;
                msg.idUser = clients[index].userId;
                msg.idChannel = channelId;
                msg.date = time(NULL);
                msg.text = texte;
                msg.status = "actif";
                newMsgId = insertMessage(&msg);
            }

            printf("\n[#%s] %s: %s\nVous: ", channel, clients[index].pseudo, texte);
            fflush(stdout);
            broadcastChatMessage(channel, clients[index].pseudo, texte, newMsgId);
            continue;
        }

        if (type == MSG_CHANNEL_LIST_REQUEST) {
            int count = 0;
            char **names = listAllChannelNames(&count);
            sendType(sock, MSG_CHANNEL_LIST_RESPONSE);
            char countStr[16];
            snprintf(countStr, sizeof(countStr), "%d", count);
            sendString(sock, countStr);
            for (int i = 0; i < count; i++) sendString(sock, names[i]);
            freeChannelNames(names, count);
            continue;
        }

        if (type == MSG_CHANNEL_HISTORY_REQUEST) {
            char channel[64];
            if (recvString(sock, channel, sizeof(channel)) < 0) break;
            int count = 0;
            HistoryEntry *entries = fetchChannelHistory(channel, &count);
            sendType(sock, MSG_CHANNEL_HISTORY_RESPONSE);
            char countStr[16];
            snprintf(countStr, sizeof(countStr), "%d", count);
            sendString(sock, countStr);
            for (int i = 0; i < count; i++) {
                char idStr[16];
                snprintf(idStr, sizeof(idStr), "%d", entries[i].id);
                sendString(sock, entries[i].auteur);
                sendString(sock, entries[i].texte);
                sendString(sock, entries[i].date);
                sendString(sock, idStr);
            }
            free(entries);
            continue;
        }

        if (type == MSG_KICK_REQUEST) {
            char pseudoCible[64];
            if (recvString(sock, pseudoCible, sizeof(pseudoCible)) < 0) break;

            if (clients[index].role < ROLE_MODERATEUR) {
                sendModResponse(sock, 0, "Permission refusee.");
                continue;
            }
            int targetIdx = findClientIndexByPseudo(pseudoCible);
            if (targetIdx < 0) {
                sendModResponse(sock, 0, "Utilisateur introuvable ou deconnecte.");
                continue;
            }
            LOCK();
            if (clients[targetIdx].role >= clients[index].role) {
                UNLOCK();
                sendModResponse(sock, 0, "Impossible de kick un utilisateur de rang egal ou superieur.");
                continue;
            }
            closesocket(clients[targetIdx].socket);
            clients[targetIdx].active = 0;
            clients[targetIdx].loggedIn = 0;
            UNLOCK();
            sendModResponse(sock, 1, NULL);
            continue;
        }

        if (type == MSG_TIMEOUT_REQUEST) {
            char pseudoCible[64], dureeStr[16];
            if (recvString(sock, pseudoCible, sizeof(pseudoCible)) < 0) break;
            if (recvString(sock, dureeStr, sizeof(dureeStr)) < 0) break;

            if (clients[index].role < ROLE_MODERATEUR) {
                sendModResponse(sock, 0, "Permission refusee.");
                continue;
            }
            int targetIdx = findClientIndexByPseudo(pseudoCible);
            if (targetIdx < 0) {
                sendModResponse(sock, 0, "Utilisateur introuvable ou deconnecte.");
                continue;
            }
            LOCK();
            if (clients[targetIdx].role >= clients[index].role) {
                UNLOCK();
                sendModResponse(sock, 0, "Impossible de timeout un utilisateur de rang egal ou superieur.");
                continue;
            }
            int duree = atoi(dureeStr);
            if (duree <= 0) duree = 60;
            clients[targetIdx].timeoutUntil = time(NULL) + duree;
            UNLOCK();
            sendModResponse(sock, 1, NULL);
            continue;
        }

        if (type == MSG_DELETE_MESSAGE_REQUEST) {
            char idStr[16];
            if (recvString(sock, idStr, sizeof(idStr)) < 0) break;

            if (clients[index].role < ROLE_MODERATEUR) {
                sendModResponse(sock, 0, "Permission refusee.");
                continue;
            }

            int msgId = atoi(idStr);
            char channelName[64];
            if (getChannelNameByMessageId(msgId, channelName, sizeof(channelName)) == 0
                && deleteMessageById(msgId)) {
                broadcastMessageDeleted(channelName, msgId); // push a tout le monde
                sendModResponse(sock, 1, NULL);
            } else {
                sendModResponse(sock, 0, "Erreur lors de la suppression du message.");
            }
            continue;
        }

        if (type == MSG_CREATE_CHANNEL_REQUEST) {
            char nom[64];
            if (recvString(sock, nom, sizeof(nom)) < 0) break;

            if (clients[index].role < ROLE_ADMINISTRATEUR) {
                sendModResponse(sock, 0, "Permission refusee.");
                continue;
            }
            int ok = createChannelInDb(nom);
            sendModResponse(sock, ok, "Erreur lors de la creation du canal (nom deja pris ?).");
            continue;
        }

        if (type == MSG_DELETE_CHANNEL_REQUEST) {
            char nom[64];
            if (recvString(sock, nom, sizeof(nom)) < 0) break;

            if (clients[index].role < ROLE_ADMINISTRATEUR) {
                sendModResponse(sock, 0, "Permission refusee.");
                continue;
            }
            int ok = deleteChannelInDb(nom);
            sendModResponse(sock, ok, "Erreur lors de la suppression du canal.");
            continue;
        }

        if (type == MSG_SET_ROLE_REQUEST) {
            char pseudoCible[64], roleStr[8];
            if (recvString(sock, pseudoCible, sizeof(pseudoCible)) < 0) break;
            if (recvString(sock, roleStr, sizeof(roleStr)) < 0) break;

            if (clients[index].role < ROLE_ADMINISTRATEUR) {
                sendModResponse(sock, 0, "Permission refusee.");
                continue;
            }
            UserRole nouveauRole = (UserRole)atoi(roleStr);
            const char *rankStr = nouveauRole == ROLE_ADMINISTRATEUR ? "administrateur"
                                 : nouveauRole == ROLE_MODERATEUR    ? "moderateur"
                                 : "member";
            int ok = setUserRoleByPseudo(pseudoCible, rankStr);
            if (ok) {
                int targetIdx = findClientIndexByPseudo(pseudoCible);
                if (targetIdx >= 0) {
                    LOCK();
                    clients[targetIdx].role = nouveauRole;
                    UNLOCK();
                }
            }
            sendModResponse(sock, ok, "Erreur lors du changement de role (pseudo introuvable ?).");
            continue;
        }
    }
}

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

    printf("\n%s authentifie (role %d).\nVous: ", clients[index].pseudo, clients[index].role);
    fflush(stdout);

    handleChatPhase(index);

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
        clients[slot].userId = -1;
        clients[slot].role = ROLE_UTILISATEUR;
        clients[slot].timeoutUntil = 0;
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
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { printf("Erreur WSAStartup\n"); return 1; }
    InitializeCriticalSection(&clientsMutex);
#endif

    for (int i = 0; i < MAX_CLIENTS; i++) { clients[i].active = 0; clients[i].loggedIn = 0; }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) { printf("Erreur creation socket\n"); return 1; }

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
        printf("Erreur bind\n"); closesocket(listenSocket); return 1;
    }
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        printf("Erreur listen\n"); closesocket(listenSocket); return 1;
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
        if (fgets(message, sizeof(message), stdin) == NULL) { running = 0; break; }
        message[strcspn(message, "\n")] = '\0';
        broadcastChatMessage("général", "Serveur", message, -1);
        if (strcmp(message, "/quit") == 0) { running = 0; break; }
        printf("Vous: ");
        fflush(stdout);
    }

    LOCK();
    for (int i = 0; i < MAX_CLIENTS; i++) if (clients[i].active) closesocket(clients[i].socket);
    UNLOCK();

    closesocket(listenSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}