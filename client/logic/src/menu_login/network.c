#include "network.h"
#include "net_utils.h"
#include "protocol.h"
#include "../../include/menu_login/login_logic.h"
#include "../../include/menu_login/register_logic.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/rand.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
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

static SOCKET sock = INVALID_SOCKET;
static MessageCallback g_msgCallback = NULL;
static ChannelCallback g_channelCallback = NULL;
static char g_currentChannel[64] = "général";
static ModResponseCallback g_modCallback = NULL;
static DeleteMessageCallback g_deleteCallback = NULL;

bool network_connect(const char *ip, int port) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return false;
#endif
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return false;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) { closesocket(sock); sock = INVALID_SOCKET; return false; }
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) { closesocket(sock); sock = INVALID_SOCKET; return false; }
    return true;
}

void network_disconnect(void) {
    if (sock != INVALID_SOCKET) { closesocket(sock); sock = INVALID_SOCKET; }
#ifdef _WIN32
    WSACleanup();
#endif
}

bool network_send_login(const char *pseudo, const char *password, char *error_msg_out, size_t err_len) {
    if (sock == INVALID_SOCKET) { snprintf(error_msg_out, err_len, "Non connecte au serveur."); return false; }

    sendType(sock, MSG_LOGIN_SALT_REQUEST);
    sendString(sock, pseudo);

    char salt_hex[SALT_LEN * 2 + 1];
    MessageType type;
    recvType(sock, &type);
    if (recvString(sock, salt_hex, sizeof(salt_hex)) < 0 || strlen(salt_hex) == 0) {
        snprintf(error_msg_out, err_len, "Pseudo inconnu.");
        return false;
    }

    unsigned char salt[SALT_LEN];
    for (int i = 0; i < SALT_LEN; i++) sscanf(salt_hex + i * 2, "%2hhx", &salt[i]);

    char hash_hex[HASH_LEN * 2 + 1];
    hash_password_with_salt(password, salt, hash_hex);

    sendType(sock, MSG_LOGIN_REQUEST);
    sendString(sock, pseudo);
    sendString(sock, hash_hex);

    char response[256];
    recvType(sock, &type);
    recvString(sock, response, sizeof(response));

    if (strcmp(response, "OK") == 0) return true;
    snprintf(error_msg_out, err_len, "%s", response);
    return false;
}

bool network_send_register(const char *nom, const char *prenom, const char *pseudo,
                            const char *email, const char *password,
                            char *error_msg_out, size_t err_len) {
    if (sock == INVALID_SOCKET) { snprintf(error_msg_out, err_len, "Non connecte au serveur."); return false; }

    unsigned char salt[SALT_LEN];
    if (!RAND_bytes(salt, SALT_LEN)) { snprintf(error_msg_out, err_len, "Erreur generation sel."); return false; }

    char salt_hex[SALT_LEN * 2 + 1];
    char hash_hex[HASH_LEN * 2 + 1];
    byte_to_hex(salt, SALT_LEN, salt_hex);
    hash_password_with_salt(password, salt, hash_hex);

    sendType(sock, MSG_REGISTER_REQUEST);
    sendString(sock, nom);
    sendString(sock, prenom);
    sendString(sock, pseudo);
    sendString(sock, email);
    sendString(sock, salt_hex);
    sendString(sock, hash_hex);

    char response[256];
    MessageType type;
    recvType(sock, &type);
    recvString(sock, response, sizeof(response));

    if (strcmp(response, "OK") == 0) return true;
    snprintf(error_msg_out, err_len, "%s", response);
    return false;
}

// Thread unique de reception : dispatch tous les types de messages entrants
// (chat, liste de canaux, historique, suppressions) vers les callbacks GTK.
static THREAD_RET listenThread(void *arg) {
    MessageType type;

    while (recvType(sock, &type) == 0) {

        if (type == MSG_CHAT_INCOMING) {
            char channel[64], auteur[64], texte[1024], idStr[16];
            if (recvString(sock, channel, sizeof(channel)) < 0) break;
            if (recvString(sock, auteur, sizeof(auteur)) < 0) break;
            if (recvString(sock, texte, sizeof(texte)) < 0) break;
            if (recvString(sock, idStr, sizeof(idStr)) < 0) break;

            int msgId = atoi(idStr);
            if (g_msgCallback) g_msgCallback(msgId, auteur, channel, texte, "--:--");
            continue;
        }

        if (type == MSG_CHANNEL_LIST_RESPONSE) {
            char countStr[16];
            if (recvString(sock, countStr, sizeof(countStr)) < 0) break;
            int count = atoi(countStr);
            for (int i = 0; i < count; i++) {
                char name[64];
                if (recvString(sock, name, sizeof(name)) < 0) break;
                if (g_channelCallback) g_channelCallback("Canaux", name);
            }
            continue;
        }

        if (type == MSG_CHANNEL_HISTORY_RESPONSE) {
            char countStr[16];
            if (recvString(sock, countStr, sizeof(countStr)) < 0) break;
            int count = atoi(countStr);
            for (int i = 0; i < count; i++) {
                char auteur[64], texte[1024], date[32], idStr[16];
                if (recvString(sock, auteur, sizeof(auteur)) < 0) break;
                if (recvString(sock, texte, sizeof(texte)) < 0) break;
                if (recvString(sock, date, sizeof(date)) < 0) break;
                if (recvString(sock, idStr, sizeof(idStr)) < 0) break;
                int msgId = atoi(idStr);
                if (g_msgCallback) g_msgCallback(msgId, auteur, g_currentChannel, texte, date);
            }
            continue;
        }

        if (type == MSG_MOD_RESPONSE) {
            char response[256];
            if (recvString(sock, response, sizeof(response)) < 0) break;
            bool ok = (strcmp(response, "OK") == 0);
            if (g_modCallback) g_modCallback(ok, ok ? "" : response);
            continue;
        }

        if (type == MSG_MESSAGE_DELETED_BROADCAST) {
            char channel[64], idStr[16];
            if (recvString(sock, channel, sizeof(channel)) < 0) break;
            if (recvString(sock, idStr, sizeof(idStr)) < 0) break;
            if (g_deleteCallback) g_deleteCallback(channel, atoi(idStr));
            continue;
        }

        // type inattendu : on arrete plutot que de desynchroniser le flux
        break;
    }

#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

void network_start_listening(MessageCallback cb) {
    g_msgCallback = cb;
    if (sock == INVALID_SOCKET) return;
#ifdef _WIN32
    HANDLE h = CreateThread(NULL, 0, listenThread, NULL, 0, NULL);
    if (h) CloseHandle(h);
#else
    pthread_t tid;
    pthread_create(&tid, NULL, listenThread, NULL);
    pthread_detach(tid);
#endif
}

void network_set_channel_callback(ChannelCallback cb) {
    g_channelCallback = cb;
}

void network_set_delete_callback(DeleteMessageCallback cb) {
    g_deleteCallback = cb;
}

void network_send_message(const char *channel, const char *text) {
    if (sock == INVALID_SOCKET) return;
    strncpy(g_currentChannel, channel, sizeof(g_currentChannel) - 1);
    g_currentChannel[sizeof(g_currentChannel) - 1] = '\0';
    sendType(sock, MSG_CHAT);
    sendString(sock, channel);
    sendString(sock, text);
}

void network_request_channels(void) {
    if (sock == INVALID_SOCKET) return;
    sendType(sock, MSG_CHANNEL_LIST_REQUEST);
}

void network_request_channel_history(const char *channel) {
    if (sock == INVALID_SOCKET) return;
    strncpy(g_currentChannel, channel, sizeof(g_currentChannel) - 1);
    g_currentChannel[sizeof(g_currentChannel) - 1] = '\0';
    sendType(sock, MSG_CHANNEL_HISTORY_REQUEST);
    sendString(sock, channel);
}

void network_set_mod_callback(ModResponseCallback cb) {
    g_modCallback = cb;
}

void network_send_kick(const char *pseudo) {
    if (sock == INVALID_SOCKET) return;
    sendType(sock, MSG_KICK_REQUEST);
    sendString(sock, pseudo);
}

void network_send_timeout(const char *pseudo, int duree) {
    if (sock == INVALID_SOCKET) return;
    char dureeStr[16];
    snprintf(dureeStr, sizeof(dureeStr), "%d", duree);
    sendType(sock, MSG_TIMEOUT_REQUEST);
    sendString(sock, pseudo);
    sendString(sock, dureeStr);
}

void network_send_delete_message(int message_id) {
    if (sock == INVALID_SOCKET) return;
    char idStr[16];
    snprintf(idStr, sizeof(idStr), "%d", message_id);
    sendType(sock, MSG_DELETE_MESSAGE_REQUEST);
    sendString(sock, idStr);
}

void network_create_channel(const char *name) {
    if (sock == INVALID_SOCKET) return;
    sendType(sock, MSG_CREATE_CHANNEL_REQUEST);
    sendString(sock, name);
}

void network_delete_channel(const char *name) {
    if (sock == INVALID_SOCKET) return;
    sendType(sock, MSG_DELETE_CHANNEL_REQUEST);
    sendString(sock, name);
}

void network_set_role(const char *pseudo, int role) {
    if (sock == INVALID_SOCKET) return;
    char roleStr[8];
    snprintf(roleStr, sizeof(roleStr), "%d", role);
    sendType(sock, MSG_SET_ROLE_REQUEST);
    sendString(sock, pseudo);
    sendString(sock, roleStr);
}

// TODO: protocole a etendre pour ces fonctionnalites (pas encore de type reseau dedie)
void network_send_update_pseudo(const char *new_pseudo) { (void)new_pseudo; }
void network_send_reaction(int message_id, const char *emoji) { (void)message_id; (void)emoji; }